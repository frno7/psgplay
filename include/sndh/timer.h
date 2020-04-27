// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#ifndef PSGPLAY_SNDH_TIMER_H
#define PSGPLAY_SNDH_TIMER_H

enum sndh_timer_type {
	SNDH_TIMER_A = 'A',
	SNDH_TIMER_B = 'B',
	SNDH_TIMER_C = 'C',
	SNDH_TIMER_D = 'D',
	SNDH_TIMER_V = 'V',	/* Vertical blank (VBL) */
};

struct sndh_timer {
	enum sndh_timer_type type;
	int frequency;
};

static inline u32 sndh_timer_to_u32(const struct sndh_timer timer)
{
	return (timer.frequency << 8) | (timer.type & 0xff);
}

static inline struct sndh_timer u32_to_sndh_timer(const u32 timer)
{
	return (struct sndh_timer) {
		.type = timer & 0xff,
		.frequency = timer >> 8,
	};
}

#endif /* PSGPLAY_SNDH_TIMER_H */
