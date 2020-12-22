// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2020 Fredrik Noring
 */

#ifndef PSGPLAY_DIGITAL_H
#define PSGPLAY_DIGITAL_H

#include <stdbool.h>
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
 * struct psgplay_digital_mixer_volume - digital mixer volume
 * @main: main volume -80..0 dB
 * @left: left volume -40..0 dB
 * @right: right volume -40..0 dB
 */
struct psgplay_digital_mixer_volume {
	int8_t main;
	int8_t left;
	int8_t right;
};

/**
 * struct psgplay_digital_mixer_tone - digital mixer tone
 * @bass: bass -12..12 dB
 * @treble: treble -12..12 dB
 */
struct psgplay_digital_mixer_tone {
	int8_t bass;
	int8_t treble;
};

/**
 * struct psgplay_digital_mixer - digital mixer
 * @volume: main, left and right volumes
 * @tone: bass and treble
 * @mix: on true mix both PSG and sampled sound, on false only sampled sound
 */
struct psgplay_digital_mixer {
	struct psgplay_digital_mixer_volume volume;
	struct psgplay_digital_mixer_tone tone;
	bool mix;
};

/**
 * struct psgplay_digital - PSG play digital sample
 * @psg: YM2149 Programmable Sound Generator (PSG)
 * @sound: Atari STE sound with 16-bit representation
 * @mixer: Atari STE volume and tone mixer
 */
struct psgplay_digital {
	struct psgplay_digital_psg psg;
	struct psgplay_digital_sound sound;
	struct psgplay_digital_mixer mixer;
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
