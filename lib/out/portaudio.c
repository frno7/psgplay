// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 * Copyright (C) 2025 Kåre Andersen
 */

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "out/portaudio.h"

#ifdef HAVE_PORTAUDIO

#include <portaudio.h>

#include "internal/assert.h"
#include "internal/compare.h"
#include "internal/fifo.h"
#include "internal/print.h"

#include "system/unix/file.h"
#include "system/unix/memory.h"

#define PORTAUDIO_BUFFER_SIZE 4096

#define PORTAUDIO_SAMPLE(s) ((struct portaudio_sample) { .lo = (s), .hi = (s) >> 8 })

struct portaudio_sample {
	u8 lo;
	s8 hi;
};

struct portaudio_stereo_sample {
	struct portaudio_sample left;
	struct portaudio_sample right;
};

struct portaudio_state {
	bool nonblocking;
	const char *output;

	PaStream *stream;

	int frequency;

	volatile bool needs_flush;

	struct portaudio_stereo_sample buffer[PORTAUDIO_BUFFER_SIZE];
	struct fifo fifo;
};

static int portaudio_callback(const void *inputBuffer, void *outputBuffer,
	unsigned long framesPerBuffer,
	const PaStreamCallbackTimeInfo *timeInfo,
	PaStreamCallbackFlags statusFlags,
	void *userData);

static void portaudio_sample_flush(struct portaudio_state *state)
{
	if (Pa_IsStreamStopped(state->stream))
		return;

	if (fifo_empty(&state->fifo))
		return;

	const struct portaudio_stereo_sample *buffer;
	const size_t r = fifo_peek(&state->fifo,
		(const void **)&buffer);

	BUG_ON(state->nonblocking);
	BUG_ON(!r);
	BUG_ON(r % sizeof(*buffer));

	const ssize_t write_count = r / sizeof(*buffer);

	PaError err = Pa_WriteStream(state->stream, buffer, write_count);
	if (err != paNoError)
		pr_fatal_error("portaudio_sample_flush: Pa_WriteStream failed: %s",
			Pa_GetErrorText(err));

	fifo_skip(&state->fifo, write_count * sizeof(*buffer));
}

static bool portaudio_sample(s16 left, s16 right, void *arg)
{
	struct portaudio_state *state = arg;

	if (fifo_full(&state->fifo)) {
		if (!state->nonblocking)
			portaudio_sample_flush(state);

		return false;
	}

	const struct portaudio_stereo_sample stereo_sample = {
		.left  = PORTAUDIO_SAMPLE(left),
		.right = PORTAUDIO_SAMPLE(right)
	};

	const size_t w = fifo_write(&state->fifo,
		&stereo_sample, sizeof(stereo_sample));

	BUG_ON(w != sizeof(stereo_sample));

	return true;
}

static bool portaudio_pause(void *arg)
{
	struct portaudio_state *state = arg;

	PaError err = Pa_StopStream(state->stream);
	if (err != paNoError)
		pr_fatal_error("portaudio_pause: Pa_StopStream failed: %s\n",
			Pa_GetErrorText(err));

	return true;
}

static bool portaudio_resume(void *arg)
{
	struct portaudio_state *state = arg;

	PaError err = Pa_StartStream(state->stream);
	if (err != paNoError)
		pr_fatal_error("portaudio_resume: Pa_StartStream failed: %s\n",
			Pa_GetErrorText(err));

	return true;
}

static void portaudio_flush(void *arg)
{
	struct portaudio_state *state = arg;

	if (state->nonblocking) {
		state->needs_flush = true;
		return;
	}

	while (!fifo_empty(&state->fifo))
		portaudio_sample_flush(state);
}

static void portaudio_drop(void *arg)
{
	struct portaudio_state *state = arg;
	PaError err;

	fifo_clear(&state->fifo);

	err = Pa_AbortStream(state->stream);
	if (err != paNoError)
		pr_fatal_error("portaudio_drop: Pa_AbortStream failed: %s\n",
			Pa_GetErrorText(err));

	err = Pa_StartStream(state->stream);
	if (err != paNoError)
		pr_fatal_error("portaudio_drop: Pa_StartStream failed: %s\n",
			Pa_GetErrorText(err));
}

static void *portaudio_open(const char *output, int frequency, bool nonblocking)
{
	PaError err;

	err = Pa_Initialize();
	if (err != paNoError)
		goto error;

	PaDeviceIndex device = Pa_GetDefaultOutputDevice();
	if (device == paNoDevice)
		pr_fatal_error(
			"portaudio_open error: No default output device detected.\n");

	const PaDeviceInfo *deviceInfo = Pa_GetDeviceInfo(device);
	if (!deviceInfo)
		pr_fatal_error("portaudio_open error: Cannot get device info.\n");

	PaStreamParameters params = {
		.device = device,
		.channelCount = 2,
		.sampleFormat = paInt16,
		.suggestedLatency = deviceInfo->defaultLowOutputLatency,
		.hostApiSpecificStreamInfo = NULL
	};

	struct portaudio_state *state = xmalloc(sizeof(struct portaudio_state));

	PaStream *stream;
	err = Pa_OpenStream(&stream, NULL, &params, frequency,
		PORTAUDIO_BUFFER_SIZE, paClipOff,
		nonblocking ? portaudio_callback : NULL,
		nonblocking ? state : NULL);
	if (err != paNoError)
		goto error;

	*state = (struct portaudio_state) {
		.nonblocking = nonblocking,
		.output = output,
		.frequency = frequency,
		.fifo = {
			.capacity = sizeof(state->buffer),
			.buffer = state->buffer
		},
		.needs_flush = false,
		.stream = stream
	};

	err = Pa_StartStream(state->stream);
	if (err != paNoError)
		goto error;

	return state;

error:
	pr_fatal_error("portaudio_open: Error initializing PortAudio: %s\n",
				Pa_GetErrorText(err));
}

static void portaudio_close(void *arg)
{
	struct portaudio_state *state = arg;

	if (state->nonblocking) {
		fifo_clear(&state->fifo);
		state->needs_flush = true;
		return;
	}

	PaError err;

	err = Pa_StopStream(state->stream);
	if (err != paNoError)
		pr_fatal_error("portaudio_close: Pa_StopStream: %s\n",
			Pa_GetErrorText(err));

	err = Pa_CloseStream(state->stream);
	if (err != paNoError)
		pr_fatal_error("portaudio_close: Pa_CloseStream: %s\n",
			Pa_GetErrorText(err));

	Pa_Terminate();

	free(state);
}

static int portaudio_callback(const void *inputBuffer, void *outputBuffer,
	unsigned long framesPerBuffer,
	const PaStreamCallbackTimeInfo *timeInfo,
	PaStreamCallbackFlags statusFlags,
	void *userData)
{
	(void)inputBuffer;
	(void)timeInfo;
	(void)statusFlags;

	struct portaudio_state *state = userData;

	size_t requested_bytes = framesPerBuffer * sizeof(struct portaudio_stereo_sample);

	memset(outputBuffer, 0, requested_bytes);

	if (state->needs_flush && fifo_empty(&state->fifo)) {
		state->needs_flush = false;
		return paComplete;
	}

	const struct portaudio_stereo_sample *in;

	size_t available = fifo_peek(&state->fifo, (const void **)&in);

	if (available > 0) {
		memcpy(outputBuffer, in, available);
		fifo_skip(&state->fifo, available);
	}

	return paContinue;
}

#endif /* HAVE_PORTAUDIO */

const struct output portaudio_output = {
#ifdef HAVE_PORTAUDIO
	.open	= portaudio_open,
	.sample	= portaudio_sample,
	.pause	= portaudio_pause,
	.resume	= portaudio_resume,
	.flush	= portaudio_flush,
	.drop	= portaudio_drop,
	.close	= portaudio_close,
#endif /* HAVE_PORTAUDIO */
};
