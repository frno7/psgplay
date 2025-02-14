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
 * Return: number of read samples, zero for end of samples indicating
 * PSG play has been stopped, or negative on failure
 */
ssize_t psgplay_read_stereo(struct psgplay *pp,
	struct psgplay_stereo *buffer, size_t count);

/**
 * psgplay_digital_to_stereo_cb - callback type to transform digital samples
 * 	into stereo samples
 * @stereo: stereo samples
 * @digital: digital samples
 * @count: number of stereo samples to transform into digital samples
 * @arg: argument supplied to psgplay_digital_to_stereo_callback()
 */
typedef void (*psgplay_digital_to_stereo_cb)(
	struct psgplay_stereo *stereo, const struct psgplay_digital *digital,
	size_t count, void *arg);

/**
 * psgplay_digital_to_stereo_empiric - empiric stereo mix of digital samples
 * @stereo: stereo samples
 * @digital: digital samples
 * @count: number of stereo samples to transform into digital samples
 * @arg: ignored, can be %NULL
 *
 * YM2149 PSG channel mix, as measured on Atari ST hardware.
 */
void psgplay_digital_to_stereo_empiric(struct psgplay_stereo *stereo,
	const struct psgplay_digital *digital, size_t count, void *arg);

/**
 * psgplay_digital_to_stereo_linear - linear stereo mix of digital samples
 * @stereo: stereo samples
 * @digital: digital samples
 * @count: number of stereo samples to transform into digital samples
 * @arg: ignored, can be %NULL
 */
void psgplay_digital_to_stereo_linear(struct psgplay_stereo *stereo,
	const struct psgplay_digital *digital, size_t count, void *arg);

/**
 * struct psgplay_psg_stereo_balance - PSG play stereo channel balance
 * @a: channel A balance, range -1 (left) .. 0 (mono) .. +1 (right)
 * @b: channel B balance, range -1 (left) .. 0 (mono) .. +1 (right)
 * @c: channel C balance, range -1 (left) .. 0 (mono) .. +1 (right)
 */
struct psgplay_psg_stereo_balance {
	float a;
	float b;
	float c;
};

/**
 * psgplay_digital_to_stereo_balance - balance stereo mix of digital samples
 * @stereo: stereo samples
 * @digital: digital samples
 * @count: number of stereo samples to transform into digital samples
 * @arg: pointer to struct psgplay_psg_stereo_balance
 */
void psgplay_digital_to_stereo_balance(struct psgplay_stereo *stereo,
	const struct psgplay_digital *digital, size_t count, void *arg);

/**
 * psgplay_digital_to_stereo_callback - invoke callback to transform digital
 * 	samples into stereo samples
 * @pp: PSG play object
 * @cb: callback
 * @arg: optional argument supplied to @cb, can be %NULL
 */
void psgplay_digital_to_stereo_callback(struct psgplay *pp,
	const psgplay_digital_to_stereo_cb cb, void *arg);

/**
 * psgplay_stereo_downsample_cb - callback type to downsample stereo samples
 * @resample: downsampled stereo samples
 * @stereo: 250 kHz stereo samples
 * @count: number of stereo samples to downsample
 * @arg: argument supplied to psgplay_stereo_downsample_callback()
 *
 * Return: number of resamples, must be equal or less than @count
 */
typedef size_t (*psgplay_stereo_downsample_cb)(
	struct psgplay_stereo *resample, const struct psgplay_stereo *stereo,
	size_t count, void *arg);

/**
 * psgplay_stereo_downsample_callback - invoke callback to downsample
 * 	stereo samples
 * @pp: PSG play object
 * @cb: callback
 * @arg: optional argument supplied to @cb, can be %NULL
 */
void psgplay_stereo_downsample_callback(struct psgplay *pp,
	const psgplay_stereo_downsample_cb cb, void *arg);

#endif /* PSGPLAY_STEREO_H */
