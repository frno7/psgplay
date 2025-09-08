// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2025 Fredrik Noring
 */

#ifndef PSGPLAY_TEST_TIMER_SNDH_H
#define PSGPLAY_TEST_TIMER_SNDH_H

sndh_tune_value_time_names(double, tune_value_time_names);

#include "test/snd-dma-alt.h"

static struct snd_dma_alt_sample sample[1];

void sndh_init(int tune)
{
	snd_dma_alt_init(sample);
}

void sndh_play()
{
	snd_dma_alt_play(sample);
}

void sndh_exit()
{
	snd_dma_alt_exit(sample);
}

#endif /* PSGPLAY_TEST_TIMER_SNDH_H */
