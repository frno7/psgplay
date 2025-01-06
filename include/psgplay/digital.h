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

#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#define PSGPLAY_BITFIELD(field, more)					\
	field;								\
	more
#elif __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define PSGPLAY_BITFIELD(field, more)					\
	more								\
	field;
#else
#error "Bitfield neither big nor little endian?"
#endif

/**
 * struct psgplay_digital_level - digital PSG level
 * @u4: 4-bit channel level (AY-3-8910)
 * @u5: 5-bit channel level (YM2149)
 * @u6: 6-bit channel level
 * @u7: 7-bit channel level
 * @u8: 8-bit channel level
 */
union psgplay_digital_level {
	struct { PSGPLAY_BITFIELD(uint8_t u4 : 4, PSGPLAY_BITFIELD(uint8_t : 4, ;)) };
	struct { PSGPLAY_BITFIELD(uint8_t u5 : 5, PSGPLAY_BITFIELD(uint8_t : 3, ;)) };
	struct { PSGPLAY_BITFIELD(uint8_t u6 : 6, PSGPLAY_BITFIELD(uint8_t : 2, ;)) };
	struct { PSGPLAY_BITFIELD(uint8_t u7 : 7, PSGPLAY_BITFIELD(uint8_t : 1, ;)) };
	uint8_t u8;
};

/**
 * struct psgplay_digital_psg - digital PSG sample
 * @lva: level of channel A
 * @lvb: level of channel B
 * @lvc: level of channel c
 */
struct psgplay_digital_psg {
	union psgplay_digital_level lva;
	union psgplay_digital_level lvb;
	union psgplay_digital_level lvc;
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
 * Return: number of read samples, zero for end of samples indicating
 * PSG play has been stopped, or negative on failure
 */
ssize_t psgplay_read_digital(struct psgplay *pp,
	struct psgplay_digital *buffer, size_t count);

/**
 * psgplay_stop_digital_at_sample - stop PSG play after a given sample index
 * @pp: PSG play object to stop
 * @index: digital sample index, from the start of the SNDH tune, to stop at
 *
 * Calling psgplay_stop_digital_at_sample() is optional, but it will allow
 * PSG play to fade out stereo samples, which prevents a sharp and often
 * audible cut-off noise.
 *
 * Note: psgplay_read_digital() will permanently return zero to indicate that
 * it has finished and there are no more samples to be read.
 *
 * See also psgplay_stop() and psgplay_stop_at_time().
 *
 * The cut-off noise is due to the YM2149 PSG unipolar signal being
 * transformed into a bipolar signal for stereo sample mixing. PSG play
 * automatically fade in stereo samples for the first 10 ms.
 */
void psgplay_stop_digital_at_sample(struct psgplay *pp, size_t index);

#endif /* PSGPLAY_DIGITAL_H */
