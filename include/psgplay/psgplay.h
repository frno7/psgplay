// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#ifndef PSGPLAY_H
#define PSGPLAY_H

#include <stddef.h>
#include <stdint.h>

#include "digital.h"

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
 * psgplay_init - initialise PSG play
 * @data: SNDH data, must not be in compressed form
 * @size: SNDH size
 * @track: subtune to play
 * @stereo_frequency: stereo sample frequency in Hz, or zero for digital reading
 *
 * Use psgplay_read_stereo() if @stereo_frequency is nonzero, otherwise
 * psgplay_read_digital().
 *
 * Return: PSG play object, which must be freed with psgplay_free(),
 * 	or %NULL on failure
 */
struct psgplay *psgplay_init(const void *data, size_t size,
	int track, int frequency);

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

/**
 * psgplay_free - free a PSG play object previously initialised
 * @pp: PSG play object to free
 */
void psgplay_free(struct psgplay *pp);

/**
 * psgplay_instruction_callback - invoke callback for every CPU instruction
 * @pp: PSG play object
 * @cb: callback
 * @arg: optional argument supplied to @cb, can be %NULL
 */
void psgplay_instruction_callback(struct psgplay *pp,
	void (*cb)(uint32_t pc, void *arg), void *arg);

#endif /* PSGPLAY_H */
