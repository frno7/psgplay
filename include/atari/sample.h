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

typedef void (*sample_f)(
	const struct psg_sample *sample, size_t count, void *arg);

#endif /* ATARI_SAMPLE_H */
