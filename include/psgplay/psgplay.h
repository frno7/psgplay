// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#ifndef PSGPLAY_H
#define PSGPLAY_H

#include <stddef.h>

struct psgplay;

struct psgplay_stereo {
	int16_t left;
	int16_t right;
};

struct psgplay *psgplay_init(const void *data, size_t size,
	int track, int frequency);

ssize_t psgplay_read_stereo(struct psgplay *pp,
	struct psgplay_stereo *buffer, size_t count);

void psgplay_free(struct psgplay *pp);

#endif /* PSGPLAY_H */
