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
 * psgplay_stop - stop a PSG play object previously initialised
 * @pp: PSG play object to stop
 *
 * Calling psgplay_stop() is optional, but it will allow PSG play to fade out
 * stereo samples, which prevents a sharp and often audible cut-off noise.
 *
 * psgplay_stop() is mostly intended for interactive use, when the stop
 * condition is not known in advance.
 *
 * Note: psgplay_read_stereo() will continue to read samples for about 10 ms,
 * after which it will permanently return zero to indicate that it has finished
 * fading out and there are no more samples to be read.
 *
 * See also psgplay_stop_at_time() and psgplay_stop_digital_at_sample().
 *
 * The cut-off noise is due to the YM2149 PSG unipolar signal being
 * transformed into a bipolar signal for stereo sample mixing. PSG play
 * automatically fade in stereo samples for the first 10 ms.
 */
void psgplay_stop(struct psgplay *pp);

/**
 * psgplay_stop_at_time - stop PSG play after a given time
 * @pp: PSG play object to stop
 * @time: time in seconds, from the start of the SNDH tune, to stop at
 *
 * Calling psgplay_stop_at_time() is optional, but it will allow PSG play
 * to prefade out stereo samples, which prevents a sharp and often audible
 * cut-off noise.
 *
 * Note: psgplay_read_stereo() will permanently return zero to indicate that
 * it has finished fading out and there are no more samples to be read.
 *
 * See also psgplay_stop() and psgplay_stop_digital_at_sample().
 *
 * The cut-off noise is due to the YM2149 PSG unipolar signal being
 * transformed into a bipolar signal for stereo sample mixing. PSG play
 * automatically fade in stereo samples for the first 10 ms.
 */
void psgplay_stop_at_time(struct psgplay *pp, float time);

/**
 * psgplay_instruction_callback - invoke callback for every CPU instruction
 * @pp: PSG play object
 * @cb: callback
 * @arg: optional argument supplied to @cb, can be %NULL
 */
void psgplay_instruction_callback(struct psgplay *pp,
	void (*cb)(uint32_t pc, void *arg), void *arg);

#endif /* PSGPLAY_H */
