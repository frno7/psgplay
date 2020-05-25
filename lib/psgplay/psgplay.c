// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "internal/compare.h"

#include "psgplay/assert.h"

#include "atari/machine.h"
#include "atari/psg.h"

#include "psgplay/assert.h"
#include "psgplay/psgplay.h"
#include "psgplay/sndh.h"

struct psgplay {
	size_t index;
	size_t count;
	size_t capacity;
	struct psgplay_stereo *buffer;

	int frequency;
	u64 psg_cycle;
	u64 downsample_sample_cycle;

	struct {
		s16 xn[8];
		int k;
	} lowpass;

	const struct machine *machine;

	int errno_;
};

static void psgplay_sample(s16 left, s16 right, struct psgplay *pp)
{
	if (pp->errno_)
		return;

	if (pp->capacity <= pp->count) {
		const size_t capacity = pp->capacity +
			max_t(size_t, pp->capacity, 1024);

		void *buffer = realloc(pp->buffer,
			capacity * sizeof(*pp->buffer));
		if (!buffer) {
			pp->errno_ = errno;
			return;
		}

		pp->buffer = buffer;
		pp->capacity = capacity;
	}

	pp->buffer[pp->count].left = left;
	pp->buffer[pp->count].right = right;
	pp->count++;
}

static void psgplay_downsample(s16 left, s16 right, struct psgplay *pp)
{
	const u64 n = (pp->frequency * pp->psg_cycle) / PSG_FREQUENCY;

	for (; pp->downsample_sample_cycle < n; pp->downsample_sample_cycle++)
		psgplay_sample(left, right, pp);

	pp->psg_cycle += 8;
}

static void psgplay_lowpass(s16 sample, void *arg)
{
	struct psgplay *pp = arg;

	pp->lowpass.xn[pp->lowpass.k++ % ARRAY_SIZE(pp->lowpass.xn)] = sample;

	s32 x = 0;
	for (int i = 0; i < ARRAY_SIZE(pp->lowpass.xn); i++)
		x += pp->lowpass.xn[i];	/* Simplistic 8 tap FIR filter. */

	const s16 y = x / ARRAY_SIZE(pp->lowpass.xn);

	psgplay_downsample(y, y, pp);
}

static u32 parse_timer(const void *data, size_t size)
{
	struct sndh_timer timer;

	if (!sndh_tag_timer(&timer, data, size))
		return 0;

	return sndh_timer_to_u32(timer);
}

struct psgplay *psgplay_init(const void *data, size_t size,
	int track, int frequency)
{
	if (frequency < 1000 || 250000 < frequency)
		return NULL;

	struct psgplay *pp = calloc(1, sizeof(struct psgplay));
	if (!pp)
		return NULL;

	pp->frequency = frequency;
	pp->machine = &atari_st;
	pp->machine->init(data, size, track, parse_timer(data, size),
		psgplay_lowpass, pp);

	return pp;
}

ssize_t psgplay_read_stereo(struct psgplay *pp,
	struct psgplay_stereo *buffer, size_t count)
{
	size_t index = 0;

	while (index < count) {
		if (pp->index == pp->count) {
			pp->index = 0;
			pp->count = 0;

			while (!pp->count)
				if (pp->errno_) {
					errno = pp->errno_;
					return -1;
				} else if (!pp->machine->run()) {
					errno = -EIO;
					return -1;
				}
		}

		const size_t n = min(count - index, pp->count - pp->index);

		memcpy(&buffer[index], &pp->buffer[pp->index],
			n * sizeof(*buffer));

		index += n;
		pp->index += n;
	}

	return index;
}

void psgplay_free(struct psgplay *pp)
{
	if (!pp)
		return;

	free(pp->buffer);
	free(pp);
}
