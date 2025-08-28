// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 * Copyright (C) 2025 Kåre Andersen
 */

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "audio/portaudio-writer.h"

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

	struct portaudio_stereo_sample buffer[PORTAUDIO_BUFFER_SIZE];
	struct fifo fifo;
};

static void portaudio_sample_flush(struct portaudio_state *state)
{
	if (Pa_IsStreamStopped(state->stream))
		return;

	if (fifo_empty(&state->fifo))
		return;

	const struct portaudio_stereo_sample *buffer;
	const size_t r = fifo_peek(&state->fifo,
		(const void **)&buffer);

	BUG_ON(!r);
	BUG_ON(r % sizeof(*buffer));

	const ssize_t write_count = r / sizeof(*buffer);
	PaError err = Pa_WriteStream(state->stream, buffer, write_count);
	if (err != paNoError)
		pr_fatal_error("PortAudio Pa_WriteStream failed: %s",
			Pa_GetErrorText(err));

	fifo_skip(&state->fifo, write_count * sizeof(*buffer));
}

static bool portaudio_sample(s16 left, s16 right, void *arg)
{
	struct portaudio_state *state = arg;

	if (fifo_full(&state->fifo)) {
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

	if (fifo_full(&state->fifo))
		portaudio_sample_flush(state);

	return true;
}

static bool portaudio_pause(void *arg)
{
	struct portaudio_state *state = arg;

	if (Pa_IsStreamStopped(state->stream))
		return true;

	PaError err = Pa_StopStream(state->stream);
	if (err != paNoError)
		pr_fatal_error("PortAudio Pa_StopStream failed: %s\n",
			Pa_GetErrorText(err));

	return true;
}

static bool portaudio_resume(void *arg)
{
	struct portaudio_state *state = arg;

	PaError err = Pa_StartStream(state->stream);
	if (err != paNoError)
		pr_fatal_error("PortAudio Pa_StartStream failed: %s\n",
			Pa_GetErrorText(err));

	return true;
}

static void portaudio_flush(void *arg)
{
	struct portaudio_state *state = arg;

	while (!fifo_empty(&state->fifo))
		portaudio_sample_flush(state);
}

static void portaudio_drop(void *arg)
{
	struct portaudio_state *state = arg;
	PaError err;

	fifo_clear(&state->fifo);

	if (Pa_IsStreamStopped(state->stream))
		return;

	err = Pa_AbortStream(state->stream);

	if (err != paNoError)
		pr_fatal_error("PortAudio Pa_AbortStream failed: %s\n",
			Pa_GetErrorText(err));
}

static void *portaudio_open(const char *output, int frequency, bool nonblocking)
{
	PaStreamParameters params = { };
	PaStream *stream;
	PaError err;

	err = Pa_Initialize();
	if (err != paNoError)
		goto error;

	params.device = Pa_GetDefaultOutputDevice();
	if (params.device == paNoDevice)
		pr_fatal_error("PortAudio error: No default output device detected.\n");

	params.channelCount = 2;
	params.sampleFormat = paInt16;
	params.suggestedLatency =
		Pa_GetDeviceInfo(params.device)->defaultLowOutputLatency;
	params.hostApiSpecificStreamInfo = NULL;

	err = Pa_OpenStream(&stream, NULL, &params, frequency,
		PORTAUDIO_BUFFER_SIZE, paClipOff, NULL, NULL);
	if (err != paNoError)
		goto error;

	struct portaudio_state *state = xmalloc(sizeof(struct portaudio_state));
	*state = (struct portaudio_state) {
		.nonblocking = nonblocking,
		.output = output,
		.stream = stream,
		.frequency = frequency,
		.fifo = {
			.capacity = sizeof(state->buffer),
			.buffer = state->buffer
		},
	};

	err = Pa_StartStream(state->stream);
	if (err != paNoError)
		goto error;

	return state;

error:
	pr_fatal_error("Error initializing PortAudio: %s\n", Pa_GetErrorText(err));
}

static void portaudio_close(void *arg)
{
	struct portaudio_state *state = arg;
	PaError err;

	portaudio_sample_flush(state);

	if (!Pa_IsStreamStopped(state->stream)) {
		err = Pa_StopStream(state->stream);
		if (err != paNoError)
			goto error;
	}

	free(state);

	err = Pa_Terminate();
	if (err == paNoError)
		return;

error:
	pr_fatal_error("Error shutting down PortAudio: %s\n", Pa_GetErrorText(err));
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
