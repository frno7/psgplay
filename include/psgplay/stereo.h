// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2022 Fredrik Noring
 */

#ifndef PSGPLAY_STEREO_H
#define PSGPLAY_STEREO_H

#include <stddef.h>
#include <stdint.h>

#include "digital.h"

struct psgplay;		/* PSG play object */

/**
 * struct psgplay_stereo - PSG play stereo sample
 * @left: 16-bit left sample
 * @right: 16-bit right sample
 */
struct psgplay_stereo {
	int16_t left;
	int16_t right;
};

/**
 * psgplay_read_stereo - read PSG play stereo samples
 * @pp: PSG play object
 * @buffer: buffer to read into, can be %NULL to ignore
 * @count: number of stereo samples to read
 *
 * Return: number of read samples, or negative on failure
 */
ssize_t psgplay_read_stereo(struct psgplay *pp,
	struct psgplay_stereo *buffer, size_t count);

#endif /* PSGPLAY_STEREO_H */
