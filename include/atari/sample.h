// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#ifndef ATARI_SAMPLE_H
#define ATARI_SAMPLE_H

#include "internal/types.h"

struct psg_sample {
	u8 lva;
	u8 lvb;
	u8 lvc;
};

typedef void (*sample_f)(const struct psg_sample *sample, void *arg);

typedef bool (*resample_f)(s16 left, s16 right,
	sample_f sample, void *sample_arg, void *resample_arg);

#endif /* ATARI_SAMPLE_H */
