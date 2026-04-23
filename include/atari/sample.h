// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#ifndef ATARI_SAMPLE_H
#define ATARI_SAMPLE_H

#include "internal/types.h"

struct cf2149_ac;

typedef void (*psg_sample_f)(
	const struct cf2149_ac *sample, size_t count, void *arg);

struct sound_sample {
	int16_t left;
	int16_t right;
};

typedef void (*sound_sample_f)(
	const struct sound_sample *sample, size_t count, void *arg);

struct mixer_sample {
	struct {
		int8_t main;
		int8_t left;
		int8_t right;
	} volume;
	struct {
		int8_t bass;
		int8_t treble;
	} tone;
	bool mix;
};

typedef void (*mixer_sample_f)(
	const struct mixer_sample *sample, size_t count, void *arg);

typedef void (*record_sample_f)(uint64_t cycle, void *arg);

#endif /* ATARI_SAMPLE_H */
