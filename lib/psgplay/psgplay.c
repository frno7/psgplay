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

struct stereo_buffer {
	size_t index;
	size_t count;
	size_t capacity;
	struct psgplay_stereo *sample;
};

struct psgplay {
	struct stereo_buffer stereo_buffer;

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

static int buffer_stereo_sample(s16 left, s16 right, struct stereo_buffer *sb)
{
	if (sb->capacity <= sb->count) {
		const size_t capacity = sb->capacity +
			max_t(size_t, sb->capacity, 1024);

		void *sample = realloc(sb->sample,
			capacity * sizeof(*sb->sample));
		if (!sample)
			return errno;

		sb->sample = sample;
		sb->capacity = capacity;
	}

	sb->sample[sb->count].left = left;
	sb->sample[sb->count].right = right;
	sb->count++;

	return 0;
}

static void psgplay_downsample(s16 left, s16 right, struct psgplay *pp)
{
	const u64 n = (pp->frequency * pp->psg_cycle) / PSG_FREQUENCY;

	for (; pp->downsample_sample_cycle < n; pp->downsample_sample_cycle++)
		if (!pp->errno_)
			pp->errno_ = buffer_stereo_sample(left, right,
				&pp->stereo_buffer);

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

	const u32 offset = MACHINE_PROGRAM;
	const struct machine_registers regs = {
		.d = { size, track, parse_timer(data, size) },
		.a = { offset },
	};

	const struct machine_ports ports = {
		.psg_sample = psg_dac3,
		.arg = pp
	};

	pp->frequency = frequency;
	pp->machine = &atari_st;
	pp->machine->init(data, size, offset, &regs, &ports);

	return pp;
}

ssize_t psgplay_read_stereo(struct psgplay *pp,
	struct psgplay_stereo *buffer, size_t count)
{
	struct stereo_buffer *sb = &pp->stereo_buffer;
	size_t index = 0;

	cpu_instruction_callback(
		pp->instruction_callback.cb,
		pp->instruction_callback.arg);

	while (index < count) {
		if (sb->index == sb->count) {
			sb->index = 0;
			sb->count = 0;

			while (!sb->count)
				if (pp->errno_) {
					errno = pp->errno_;
					return -1;
				} else if (!pp->machine->run()) {
					errno = -EIO;
					return -1;
				}
		}

		const size_t n = min(count - index, sb->count - sb->index);

		if (buffer != NULL)
			memcpy(&buffer[index], &sb->sample[sb->index],
				n * sizeof(*buffer));

		index += n;
		sb->index += n;
	}

	return index;
}

void psgplay_free(struct psgplay *pp)
{
	if (!pp)
		return;

	free(pp->stereo_buffer.sample);
	free(pp);
}

void psgplay_instruction_callback(struct psgplay *pp,
	void (*cb)(uint32_t pc, void *arg), void *arg)
{
	pp->instruction_callback.cb = cb;
	pp->instruction_callback.arg = arg;
}
