// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2020 Fredrik Noring
 */

#ifndef PSGPLAY_DIGITAL_H
#define PSGPLAY_DIGITAL_H

#include <stddef.h>
#include <stdint.h>

struct psgplay;		/* PSG play object */

/**
 * struct psgplay_digital_psg - digital PSG sample
 * @lva: 0..15 level of channel A
 * @lvb: 0..15 level of channel B
 * @lvc: 0..15 level of channel c
 */
struct psgplay_digital_psg {
	uint8_t lva;
	uint8_t lvb;
	uint8_t lvc;
};

/**
 * struct psgplay_digital_sound - digital sound sample
 * @left: -32768..32767 level of left channel
 * @right: -32768..32767 level of right channel
 */
struct psgplay_digital_sound {
	int16_t left;
	int16_t right;
};

/**
 * struct psgplay_digital - PSG play digital sample
 * @psg: YM2149 Programmable Sound Generator (PSG)
 * @sound: Atari STE sound with 16-bit representation
 */
struct psgplay_digital {
	struct psgplay_digital_psg psg;
	struct psgplay_digital_sound sound;
};

/**
 * psgplay_read_digital - read 250 kHz PSG play digital samples
 * @pp: PSG play object
 * @buffer: buffer to read into, can be %NULL to ignore
 * @count: number of digital samples to read
 *
 * Return: number of read samples, or negative on failure
 */
ssize_t psgplay_read_digital(struct psgplay *pp,
	struct psgplay_digital *buffer, size_t count);

#endif /* PSGPLAY_DIGITAL_H */
