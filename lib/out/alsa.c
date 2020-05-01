// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#include <stdlib.h>
#include <unistd.h>

#include "out/alsa.h"

#ifdef HAVE_ALSA

#include <alsa/asoundlib.h>

#include "internal/compare.h"

#include "psgplay/assert.h"
#include "psgplay/print.h"

#include "system/unix/file.h"
#include "system/unix/memory.h"

#define ALSA_SAMPLE(s) ((struct alsa_sample) { .lo = (s), .hi = (s) >> 8 })

struct alsa_sample {
	u8 lo;
	s8 hi;
};

struct alsa_state {
	const char *output;

	snd_pcm_t *pcm_handle;
	snd_pcm_hw_params_t *hwparams;

	int frequency;

	size_t sample_count;

	size_t buffer_count;
	struct {
		struct alsa_sample left;
		struct alsa_sample right;
	} buffer[16384];
};

static bool alsa_sample_flush(struct alsa_state *state)
{
	const size_t buffer_size = state->buffer_count * sizeof(*state->buffer);
	const ssize_t frame_count = snd_pcm_writei(
		state->pcm_handle, state->buffer, state->buffer_count);

	BUG_ON(4 * ARRAY_SIZE(state->buffer) != sizeof(state->buffer));

	state->buffer_count = 0;

	if (frame_count < 0)
		pr_fatal_error("%s: ALSA snd_pcm_writei failed: %s\n",
			state->output, snd_strerror(frame_count));
	else if (frame_count * 4 != buffer_size)
		pr_fatal_error("%s: ALSA snd_pcm_writei failed: Incomplete samples\n",
			state->output);

	return true;
}

static bool alsa_sample(s16 left, s16 right, void *arg)
{
	struct alsa_state *state = arg;

	state->sample_count++;

	state->buffer[state->buffer_count].left  = ALSA_SAMPLE(left);
	state->buffer[state->buffer_count].right = ALSA_SAMPLE(right);
	state->buffer_count++;

	if (state->buffer_count < ARRAY_SIZE(state->buffer))
		return true;

	return alsa_sample_flush(state);
}

static void *alsa_open(const char *output, int track, int frequency)
{
	struct alsa_state *state = xmalloc(sizeof(struct alsa_state));
	int err;

	*state = (struct alsa_state) {
		.output = output,
		.frequency = frequency,
	};

	snd_pcm_hw_params_alloca(&state->hwparams);

	err = snd_pcm_open(&state->pcm_handle,
		"plughw:0,0", SND_PCM_STREAM_PLAYBACK, 0);
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

	err = snd_pcm_hw_params(state->pcm_handle, state->hwparams);
	if (err < 0)
		pr_fatal_error("%s: ALSA snd_pcm_hw_params failed: %s\n",
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

const struct output alsa_output = {
#ifdef HAVE_ALSA
	.open	= alsa_open,
	.sample	= alsa_sample,
	.close	= alsa_close,
#endif /* HAVE_ALSA */
};
