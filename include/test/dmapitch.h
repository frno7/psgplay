// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2025 Fredrik Noring
 */

#ifndef PSGPLAY_TEST_DMAPITCH_H
#define PSGPLAY_TEST_DMAPITCH_H

struct dma_preset {
	int channels;
	int halfperiod;
	int rate;
};

#define D(c, hp, r) { .channels = (c), .halfperiod = (hp), .rate = (r) }

#define tune_value_time_names(t)					\
	t(D(1, 1, 0), 63, "DMA mono halfperiod 1 at 6258 Hz")		\
	t(D(1, 2, 0), 63, "DMA mono halfperiod 2 at 6258 Hz")		\
	t(D(1, 3, 0), 63, "DMA mono halfperiod 3 at 6258 Hz")		\
	t(D(1, 5, 0), 63, "DMA mono halfperiod 5 at 6258 Hz")		\
	t(D(1, 7, 0), 63, "DMA mono halfperiod 7 at 6258 Hz")		\
	t(D(1, 1, 1), 63, "DMA mono halfperiod 1 at 12517 Hz")		\
	t(D(1, 2, 1), 63, "DMA mono halfperiod 2 at 12517 Hz")		\
	t(D(1, 3, 1), 63, "DMA mono halfperiod 3 at 12517 Hz")		\
	t(D(1, 5, 1), 63, "DMA mono halfperiod 5 at 12517 Hz")		\
	t(D(1, 7, 1), 63, "DMA mono halfperiod 7 at 12517 Hz")		\
	t(D(1, 1, 2), 63, "DMA mono halfperiod 1 at 25033 Hz")		\
	t(D(1, 2, 2), 63, "DMA mono halfperiod 2 at 25033 Hz")		\
	t(D(1, 3, 2), 63, "DMA mono halfperiod 3 at 25033 Hz")		\
	t(D(1, 5, 2), 63, "DMA mono halfperiod 5 at 25033 Hz")		\
	t(D(1, 7, 2), 63, "DMA mono halfperiod 7 at 25033 Hz")		\
	t(D(1, 2, 3), 63, "DMA mono halfperiod 2 at 50066 Hz")		\
	t(D(1, 3, 3), 63, "DMA mono halfperiod 3 at 50066 Hz")		\
	t(D(1, 5, 3), 63, "DMA mono halfperiod 5 at 50066 Hz")		\
	t(D(1, 7, 3), 63, "DMA mono halfperiod 7 at 50066 Hz")		\
	t(D(2, 1, 0), 63, "DMA stereo halfperiod 1 at 6258 Hz")		\
	t(D(2, 2, 0), 63, "DMA stereo halfperiod 2 at 6258 Hz")		\
	t(D(2, 3, 0), 63, "DMA stereo halfperiod 3 at 6258 Hz")		\
	t(D(2, 5, 0), 63, "DMA stereo halfperiod 5 at 6258 Hz")		\
	t(D(2, 7, 0), 63, "DMA stereo halfperiod 7 at 6258 Hz")		\
	t(D(2, 1, 1), 63, "DMA stereo halfperiod 1 at 12517 Hz")	\
	t(D(2, 2, 1), 63, "DMA stereo halfperiod 2 at 12517 Hz")	\
	t(D(2, 3, 1), 63, "DMA stereo halfperiod 3 at 12517 Hz")	\
	t(D(2, 5, 1), 63, "DMA stereo halfperiod 5 at 12517 Hz")	\
	t(D(2, 7, 1), 63, "DMA stereo halfperiod 7 at 12517 Hz")	\
	t(D(2, 1, 2), 63, "DMA stereo halfperiod 1 at 25033 Hz")	\
	t(D(2, 2, 2), 63, "DMA stereo halfperiod 2 at 25033 Hz")	\
	t(D(2, 3, 2), 63, "DMA stereo halfperiod 3 at 25033 Hz")	\
	t(D(2, 5, 2), 63, "DMA stereo halfperiod 5 at 25033 Hz")	\
	t(D(2, 7, 2), 63, "DMA stereo halfperiod 7 at 25033 Hz")	\
	t(D(2, 2, 3), 63, "DMA stereo halfperiod 2 at 50066 Hz")	\
	t(D(2, 3, 3), 63, "DMA stereo halfperiod 3 at 50066 Hz")	\
	t(D(2, 5, 3), 63, "DMA stereo halfperiod 5 at 50066 Hz")	\
	t(D(2, 7, 3), 63, "DMA stereo halfperiod 7 at 50066 Hz")

#endif /* PSGPLAY_TEST_DMAPITCH_H */
