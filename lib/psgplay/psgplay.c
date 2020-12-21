// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "internal/assert.h"
#include "internal/compare.h"

#include "atari/cpu.h"
#include "atari/machine.h"
#include "atari/psg.h"

#include "psgplay/psgplay.h"
#include "psgplay/sndh.h"

struct fir8 {
	s16 xn[8];
	int k;
};

struct psgplay {
	size_t index;
	size_t count;
	size_t capacity;
	struct psgplay_stereo *buffer;

	int frequency;
	u64 psg_cycle;
	u64 downsample_sample_cycle;

	struct fir8 lowpass;

	const struct machine *machine;

	struct {
		void (*cb)(uint32_t pc, void *arg);
		void *arg;
	} instruction_callback;

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

static s16 sample_lowpass(s16 sample, struct fir8 *lowpass)
{
	lowpass->xn[lowpass->k++ % ARRAY_SIZE(lowpass->xn)] = sample;

	s32 x = 0;
	for (int i = 0; i < ARRAY_SIZE(lowpass->xn); i++)
		x += lowpass->xn[i];	/* Simplistic 8 tap FIR filter. */

	return x / ARRAY_SIZE(lowpass->xn);
}

static s16 psg_dac(const u8 level)
{
	/*
	 * Table is computed with 2^[ (lvl-15)/2 ] although the levels in
	 * figure 9 in AY-3-8910/8912 Programmable Sound Generator Data Manual,
	 * February 1979, p. 29 are slightly different as shown in the comment.
	 */

	static const u16 dac[16] = {
		0.006 * 0xffff,		/* 0.000 */
		0.008 * 0xffff,		/* ..... */
		0.011 * 0xffff,		/* ..... */
		0.016 * 0xffff,		/* ..... */
		0.022 * 0xffff,		/* ..... */
		0.031 * 0xffff,		/* ..... */
		0.044 * 0xffff,		/* ..... */
		0.062 * 0xffff,		/* ..... */
		0.088 * 0xffff,		/* ..... */
		0.125 * 0xffff,		/* 0.125 */
		0.177 * 0xffff,		/* 0.152 */
		0.250 * 0xffff,		/* 0.250 */
		0.354 * 0xffff,		/* 0.303 */
		0.500 * 0xffff,		/* 0.500 */
		0.707 * 0xffff,		/* 0.707 */
		1.000 * 0xffff,		/* 1.000 */
	};

	return (level < 16 ? dac[level] : 0xffff) - 0x8000;
}

static void psg_dac3(const struct psg_sample *sample, size_t count, void *arg)
{
	for (size_t i = 0; i < count; i++) {
		struct psgplay *pp = arg;

		const s16 sa = psg_dac(sample[i].lva);
		const s16 sb = psg_dac(sample[i].lvb);
		const s16 sc = psg_dac(sample[i].lvc);

		/* Simplistic linear channel mix. */
		const s16 s = (sa + sb + sc) / 3;

		const s16 y = sample_lowpass(s, &pp->lowpass);

		psgplay_downsample(y, y, pp);
	}
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
		psg_dac3, pp);

	return pp;
}

ssize_t psgplay_read_stereo(struct psgplay *pp,
	struct psgplay_stereo *buffer, size_t count)
{
	size_t index = 0;

	cpu_instruction_callback(
		pp->instruction_callback.cb,
		pp->instruction_callback.arg);

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

		if (buffer != NULL)
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

void psgplay_instruction_callback(struct psgplay *pp,
	void (*cb)(uint32_t pc, void *arg), void *arg)
{
	pp->instruction_callback.cb = cb;
	pp->instruction_callback.arg = arg;
}
