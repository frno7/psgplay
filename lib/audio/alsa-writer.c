// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "audio/alsa-writer.h"

#ifdef HAVE_ALSA

#include <alsa/asoundlib.h>

#include "internal/assert.h"
#include "internal/compare.h"
#include "internal/fifo.h"
#include "internal/print.h"

#include "system/unix/file.h"
#include "system/unix/memory.h"

#define ALSA_SAMPLE(s) ((struct alsa_sample) { .lo = (s), .hi = (s) >> 8 })

struct alsa_sample {
	u8 lo;
	s8 hi;
};

struct alsa_stereo_sample {
	struct alsa_sample left;
	struct alsa_sample right;
};

struct alsa_state {
	bool nonblocking;
	const char *output;

	snd_pcm_t *pcm_handle;
	snd_pcm_hw_params_t *hwparams;

	int frequency;

	struct alsa_stereo_sample buffer[4096];
	struct fifo fifo;
};

static void alsa_sample_flush(struct alsa_state *state)
{
	while (!fifo_empty(&state->fifo)) {
		const struct alsa_stereo_sample *buffer;
		const size_t r = fifo_peek(&state->fifo,
			(const void **)&buffer);

		BUG_ON(!r);
		BUG_ON(r % sizeof(*buffer));

		const ssize_t write_count = r / sizeof(*buffer);
		const ssize_t frame_count = snd_pcm_writei(
			state->pcm_handle, buffer, write_count);

		if (!frame_count || frame_count == -EAGAIN)
			break;

		if (frame_count < 0)
			pr_fatal_error("%s: ALSA snd_pcm_writei failed: %zd %s\n",
				state->output, frame_count,
				snd_strerror(frame_count));

		fifo_skip(&state->fifo, frame_count * sizeof(*buffer));

		if (frame_count < write_count)
			break;
	}
}

static bool alsa_sample(s16 left, s16 right, void *arg)
{
	struct alsa_state *state = arg;

	if (fifo_full(&state->fifo)) {
		alsa_sample_flush(state);

		return false;
	}

	const struct alsa_stereo_sample stereo_sample = {
		.left  = ALSA_SAMPLE(left),
		.right = ALSA_SAMPLE(right)
	};

	const size_t w = fifo_write(&state->fifo,
		&stereo_sample, sizeof(stereo_sample));

	BUG_ON(w != sizeof(stereo_sample));

	if (fifo_full(&state->fifo))
		alsa_sample_flush(state);

	return true;
}

static bool alsa_pause(void *arg)
{
	struct alsa_state *state = arg;
	int err;

	err = snd_pcm_pause(state->pcm_handle, true);
	if (err < 0)
		pr_fatal_error("%s: ALSA snd_pcm_pause failed: %s\n",
			state->output, snd_strerror(err));

	return true;
}

static bool alsa_resume(void *arg)
{
	struct alsa_state *state = arg;
	int err;

	err = snd_pcm_pause(state->pcm_handle, false);
	if (err < 0)
		pr_fatal_error("%s: ALSA snd_pcm_pause failed: %s\n",
			state->output, snd_strerror(err));

	return true;
}

static void alsa_flush(void *arg)
{
	struct alsa_state *state = arg;

	snd_pcm_nonblock(state->pcm_handle, 0);

	while (!fifo_empty(&state->fifo))
		alsa_sample_flush(state);

	int err = snd_pcm_drain(state->pcm_handle);
	if (err < 0)
		pr_fatal_error("%s: ALSA snd_pcm_drain failed: %s\n",
			state->output, snd_strerror(err));

	if (state->nonblocking)
		snd_pcm_nonblock(state->pcm_handle, SND_PCM_NONBLOCK);
}

static void alsa_drop(void *arg)
{
	struct alsa_state *state = arg;
	int err;

	fifo_clear(&state->fifo);

	err = snd_pcm_drop(state->pcm_handle);
	if (err < 0)
		pr_fatal_error("%s: ALSA snd_pcm_drop failed: %s\n",
			state->output, snd_strerror(err));

	err = snd_pcm_prepare(state->pcm_handle);
	if (err < 0)
		pr_fatal_error("%s: ALSA snd_pcm_prepare failed: %s\n",
			state->output, snd_strerror(err));
}

static void *alsa_open(const char *output, int frequency, bool nonblocking)
{
	struct alsa_state *state = xmalloc(sizeof(struct alsa_state));
	int err;

	*state = (struct alsa_state) {
		.nonblocking = nonblocking,
		.output = output,
		.frequency = frequency,
		.fifo = {
			.capacity = sizeof(state->buffer),
			.buffer = state->buffer
		},
	};

	snd_pcm_hw_params_alloca(&state->hwparams);

	err = snd_pcm_open(&state->pcm_handle,
		alsa_writer_handle(output), SND_PCM_STREAM_PLAYBACK,
		nonblocking ? SND_PCM_NONBLOCK : 0);
	if (err < 0)
		pr_fatal_error("%s: ALSA snd_pcm_open failed: %s\n",
			output, snd_strerror(err));

	err = snd_pcm_hw_params_any(state->pcm_handle, state->hwparams);
	if (err < 0)
		pr_fatal_error("%s: ALSA snd_pcm_hw_params_any failed: %s\n",
			output, snd_strerror(err));

	err = snd_pcm_hw_params_set_access(state->pcm_handle,
		state->hwparams, SND_PCM_ACCESS_RW_INTERLEAVED);
	if (err < 0)
		pr_fatal_error("%s: ALSA snd_pcm_hw_params_set_access failed: %s\n",
			output, snd_strerror(err));

	err = snd_pcm_hw_params_set_format(state->pcm_handle,
		state->hwparams, SND_PCM_FORMAT_S16_LE);
	if (err < 0)
		pr_fatal_error("%s: ALSA snd_pcm_hw_params_set_format failed: %s\n",
			output, snd_strerror(err));

	err = snd_pcm_hw_params_set_rate(state->pcm_handle,
		state->hwparams, frequency, 0);
	if (err < 0)
		pr_fatal_error("%s: ALSA snd_pcm_hw_params_set_rate failed: %s\n",
			output, snd_strerror(err));

	err = snd_pcm_hw_params_set_channels(state->pcm_handle,
		state->hwparams, 2);
	if (err < 0)
		pr_fatal_error("%s: ALSA snd_pcm_hw_params_set_channels failed: %s\n",
			output, snd_strerror(err));

	/*
	 * The 100 ms audio buffer time is chosen so that for example
	 * interactively changing the volume is reasonably responsive. An
	 * assumption is that the nonblocking case, primarily the interactive
	 * text mode, updates the buffer in approximately 20 ms cycles.
	 *
	 * The blocking case could have a much larger buffer, primarily in
	 * the command mode, since it's noninteractive, and thereby become
	 * much more resilient against process scheduling problems causing
	 * choppy audio due to high system load and performance problems.
	 */
	if (nonblocking) {
		unsigned int buffer_time = 100000;	/* 100 ms */
		err = snd_pcm_hw_params_set_buffer_time_near(state->pcm_handle,
			state->hwparams, &buffer_time, NULL);
		if (err < 0)
			pr_fatal_error("%s: ALSA snd_pcm_hw_params_set_buffer_time_near failed: %s\n",
				output, snd_strerror(err));
	}

	err = snd_pcm_hw_params(state->pcm_handle, state->hwparams);
	if (err < 0)
		pr_fatal_error("%s: ALSA snd_pcm_hw_params failed: %s\n",
			output, snd_strerror(err));

	err = snd_pcm_prepare(state->pcm_handle);
	if (err < 0)
		pr_fatal_error("%s: ALSA snd_pcm_prepare failed: %s\n",
			output, snd_strerror(err));

	return state;
}

static void alsa_close(void *arg)
{
	struct alsa_state *state = arg;

	alsa_sample_flush(state);

	snd_pcm_drain(state->pcm_handle);
	snd_pcm_close(state->pcm_handle);

	free(state);
}

#endif /* HAVE_ALSA */

const char *alsa_writer_handle(const char *output)
{
	return !output ? "default" :
		strncmp(output, "alsa:", 5) == 0 ? &output[5] : NULL;
}

const struct audio_writer alsa_writer = {
#ifdef HAVE_ALSA
	.open	= alsa_open,
	.sample	= alsa_sample,
	.pause	= alsa_pause,
	.resume	= alsa_resume,
	.flush	= alsa_flush,
	.drop	= alsa_drop,
	.close	= alsa_close,
#endif /* HAVE_ALSA */
};
