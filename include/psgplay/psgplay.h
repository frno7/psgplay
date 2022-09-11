// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#ifndef PSGPLAY_H
#define PSGPLAY_H

#include <stddef.h>
#include <stdint.h>

#include "digital.h"
#include "stereo.h"

/**
 * psgplay_init - initialise PSG play
 * @data: SNDH data, must not be in compressed form
 * @size: SNDH size in octets
 * @track: subtune to play
 * @frequency: stereo sample frequency in Hz, or zero for digital reading
 *
 * Use psgplay_read_stereo() if @frequency is nonzero, otherwise use
 * psgplay_read_digital().
 *
 * Return: PSG play object, which must be freed with psgplay_free(),
 * 	or %NULL on failure
 */
struct psgplay *psgplay_init(const void *data, size_t size,
	int track, int frequency);

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
