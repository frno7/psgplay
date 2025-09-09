// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2025 Fredrik Noring
 */

#ifndef PSGPLAY_TEST_DMASINT_H
#define PSGPLAY_TEST_DMASINT_H

struct dma_preset {
	bool timer_a;
	int channels;
	int rate;
	int halfperiod;
	int count[3];
};

#define FG(t, c, r, hp, x_, y_, z_)					\
	{								\
		.timer_a = (t),						\
		.channels = (c),					\
		.rate = (r),						\
		.halfperiod = (hp),					\
		.count = { (x_), (y_), (z_) },				\
	}

#define F(...) FG(1, __VA_ARGS__)
#define G(...) FG(0, __VA_ARGS__)

#define tune_value_time_names(t)					\
	t(F(1, 0,   8, 1, 1, 1), 4, "DMA timer A mono at 6258 Hz")	\
	t(F(1, 0,   9, 1, 1, 1), 4, "DMA timer A mono at 6258 Hz")	\
	t(F(1, 0,  10, 1, 1, 1), 4, "DMA timer A mono at 6258 Hz")	\
	t(F(1, 0,  11, 1, 1, 1), 4, "DMA timer A mono at 6258 Hz")	\
	t(F(1, 0,  32, 1, 1, 1), 4, "DMA timer A mono at 6258 Hz")	\
	t(F(1, 0,  64, 1, 1, 1), 4, "DMA timer A mono at 6258 Hz")	\
	t(F(1, 0, 128, 1, 1, 1), 4, "DMA timer A mono at 6258 Hz")	\
									\
	t(F(1, 0,   8, 5, 3, 4), 4, "DMA timer A mono at 6258 Hz")	\
	t(F(1, 0,   9, 7, 2, 3), 4, "DMA timer A mono at 6258 Hz")	\
	t(F(1, 0,  10, 3, 4, 2), 4, "DMA timer A mono at 6258 Hz")	\
	t(F(1, 0,  11, 4, 5, 6), 4, "DMA timer A mono at 6258 Hz")	\
	t(F(1, 0,  32, 2, 7, 3), 4, "DMA timer A mono at 6258 Hz")	\
	t(F(1, 0,  64, 6, 2, 4), 4, "DMA timer A mono at 6258 Hz")	\
	t(F(1, 0, 128, 4, 6, 7), 4, "DMA timer A mono at 6258 Hz")	\
									\
	t(F(1, 1,   8, 1, 1, 1), 4, "DMA timer A mono at 12517 Hz")	\
	t(F(1, 1,   9, 1, 1, 1), 4, "DMA timer A mono at 12517 Hz")	\
	t(F(1, 1,  10, 1, 1, 1), 4, "DMA timer A mono at 12517 Hz")	\
	t(F(1, 1,  11, 1, 1, 1), 4, "DMA timer A mono at 12517 Hz")	\
	t(F(1, 1,  32, 1, 1, 1), 4, "DMA timer A mono at 12517 Hz")	\
	t(F(1, 1,  64, 1, 1, 1), 4, "DMA timer A mono at 12517 Hz")	\
	t(F(1, 1, 128, 1, 1, 1), 4, "DMA timer A mono at 12517 Hz")	\
									\
	t(F(1, 1,   8, 5, 3, 4), 4, "DMA timer A mono at 12517 Hz")	\
	t(F(1, 1,   9, 7, 2, 3), 4, "DMA timer A mono at 12517 Hz")	\
	t(F(1, 1,  10, 3, 4, 2), 4, "DMA timer A mono at 12517 Hz")	\
	t(F(1, 1,  11, 4, 5, 6), 4, "DMA timer A mono at 12517 Hz")	\
	t(F(1, 1,  32, 2, 7, 3), 4, "DMA timer A mono at 12517 Hz")	\
	t(F(1, 1,  64, 6, 2, 4), 4, "DMA timer A mono at 12517 Hz")	\
	t(F(1, 1, 128, 4, 6, 7), 4, "DMA timer A mono at 12517 Hz")	\
									\
	t(F(2, 0,   8, 1, 1, 1), 4, "DMA timer A stereo at 6258 Hz")	\
	t(F(2, 0,   9, 1, 1, 1), 4, "DMA timer A stereo at 6258 Hz")	\
	t(F(2, 0,  10, 1, 1, 1), 4, "DMA timer A stereo at 6258 Hz")	\
	t(F(2, 0,  11, 1, 1, 1), 4, "DMA timer A stereo at 6258 Hz")	\
	t(F(2, 0,  32, 1, 1, 1), 4, "DMA timer A stereo at 6258 Hz")	\
	t(F(2, 0,  64, 1, 1, 1), 4, "DMA timer A stereo at 6258 Hz")	\
	t(F(2, 0, 128, 1, 1, 1), 4, "DMA timer A stereo at 6258 Hz")	\
									\
	t(F(2, 0,   8, 5, 3, 4), 4, "DMA timer A stereo at 6258 Hz")	\
	t(F(2, 0,   9, 7, 2, 3), 4, "DMA timer A stereo at 6258 Hz")	\
	t(F(2, 0,  10, 3, 4, 2), 4, "DMA timer A stereo at 6258 Hz")	\
	t(F(2, 0,  11, 4, 5, 6), 4, "DMA timer A stereo at 6258 Hz")	\
	t(F(2, 0,  32, 2, 7, 3), 4, "DMA timer A stereo at 6258 Hz")	\
	t(F(2, 0,  64, 6, 2, 4), 4, "DMA timer A stereo at 6258 Hz")	\
	t(F(2, 0, 128, 4, 6, 7), 4, "DMA timer A stereo at 6258 Hz")	\
									\
	t(F(2, 1,   8, 1, 1, 1), 4, "DMA timer A stereo at 12517 Hz")	\
	t(F(2, 1,   9, 1, 1, 1), 4, "DMA timer A stereo at 12517 Hz")	\
	t(F(2, 1,  10, 1, 1, 1), 4, "DMA timer A stereo at 12517 Hz")	\
	t(F(2, 1,  11, 1, 1, 1), 4, "DMA timer A stereo at 12517 Hz")	\
	t(F(2, 1,  32, 1, 1, 1), 4, "DMA timer A stereo at 12517 Hz")	\
	t(F(2, 1,  64, 1, 1, 1), 4, "DMA timer A stereo at 12517 Hz")	\
	t(F(2, 1, 128, 1, 1, 1), 4, "DMA timer A stereo at 12517 Hz")	\
									\
	t(F(2, 1,   8, 5, 3, 4), 4, "DMA timer A stereo at 12517 Hz")	\
	t(F(2, 1,   9, 7, 2, 3), 4, "DMA timer A stereo at 12517 Hz")	\
	t(F(2, 1,  10, 3, 4, 2), 4, "DMA timer A stereo at 12517 Hz")	\
	t(F(2, 1,  11, 4, 5, 6), 4, "DMA timer A stereo at 12517 Hz")	\
	t(F(2, 1,  32, 2, 7, 3), 4, "DMA timer A stereo at 12517 Hz")	\
	t(F(2, 1,  64, 6, 2, 4), 4, "DMA timer A stereo at 12517 Hz")	\
	t(F(2, 1, 128, 4, 6, 7), 4, "DMA timer A stereo at 12517 Hz")	\
									\
	t(G(1, 0,   8, 1, 1, 1), 4, "DMA GPIP I7 mono at 6258 Hz")	\
	t(G(1, 0,   9, 1, 1, 1), 4, "DMA GPIP I7 mono at 6258 Hz")	\
	t(G(1, 0,  10, 1, 1, 1), 4, "DMA GPIP I7 mono at 6258 Hz")	\
	t(G(1, 0,  11, 1, 1, 1), 4, "DMA GPIP I7 mono at 6258 Hz")	\
	t(G(1, 0,  32, 1, 1, 1), 4, "DMA GPIP I7 mono at 6258 Hz")	\
	t(G(1, 0,  64, 1, 1, 1), 4, "DMA GPIP I7 mono at 6258 Hz")	\
	t(G(1, 0, 128, 1, 1, 1), 4, "DMA GPIP I7 mono at 6258 Hz")	\
									\
	t(G(1, 0,   8, 5, 3, 4), 4, "DMA GPIP I7 mono at 6258 Hz")	\
	t(G(1, 0,   9, 7, 2, 3), 4, "DMA GPIP I7 mono at 6258 Hz")	\
	t(G(1, 0,  10, 3, 4, 2), 4, "DMA GPIP I7 mono at 6258 Hz")	\
	t(G(1, 0,  11, 4, 5, 6), 4, "DMA GPIP I7 mono at 6258 Hz")	\
	t(G(1, 0,  32, 2, 7, 3), 4, "DMA GPIP I7 mono at 6258 Hz")	\
	t(G(1, 0,  64, 6, 2, 4), 4, "DMA GPIP I7 mono at 6258 Hz")	\
	t(G(1, 0, 128, 4, 6, 7), 4, "DMA GPIP I7 mono at 6258 Hz")	\
									\
	t(G(1, 1,   8, 1, 1, 1), 4, "DMA GPIP I7 mono at 12517 Hz")	\
	t(G(1, 1,   9, 1, 1, 1), 4, "DMA GPIP I7 mono at 12517 Hz")	\
	t(G(1, 1,  10, 1, 1, 1), 4, "DMA GPIP I7 mono at 12517 Hz")	\
	t(G(1, 1,  11, 1, 1, 1), 4, "DMA GPIP I7 mono at 12517 Hz")	\
	t(G(1, 1,  32, 1, 1, 1), 4, "DMA GPIP I7 mono at 12517 Hz")	\
	t(G(1, 1,  64, 1, 1, 1), 4, "DMA GPIP I7 mono at 12517 Hz")	\
	t(G(1, 1, 128, 1, 1, 1), 4, "DMA GPIP I7 mono at 12517 Hz")	\
									\
	t(G(1, 1,   8, 5, 3, 4), 4, "DMA GPIP I7 mono at 12517 Hz")	\
	t(G(1, 1,   9, 7, 2, 3), 4, "DMA GPIP I7 mono at 12517 Hz")	\
	t(G(1, 1,  10, 3, 4, 2), 4, "DMA GPIP I7 mono at 12517 Hz")	\
	t(G(1, 1,  11, 4, 5, 6), 4, "DMA GPIP I7 mono at 12517 Hz")	\
	t(G(1, 1,  32, 2, 7, 3), 4, "DMA GPIP I7 mono at 12517 Hz")	\
	t(G(1, 1,  64, 6, 2, 4), 4, "DMA GPIP I7 mono at 12517 Hz")	\
	t(G(1, 1, 128, 4, 6, 7), 4, "DMA GPIP I7 mono at 12517 Hz")	\
									\
	t(G(2, 0,   8, 1, 1, 1), 4, "DMA GPIP I7 stereo at 6258 Hz")	\
	t(G(2, 0,   9, 1, 1, 1), 4, "DMA GPIP I7 stereo at 6258 Hz")	\
	t(G(2, 0,  10, 1, 1, 1), 4, "DMA GPIP I7 stereo at 6258 Hz")	\
	t(G(2, 0,  11, 1, 1, 1), 4, "DMA GPIP I7 stereo at 6258 Hz")	\
	t(G(2, 0,  32, 1, 1, 1), 4, "DMA GPIP I7 stereo at 6258 Hz")	\
	t(G(2, 0,  64, 1, 1, 1), 4, "DMA GPIP I7 stereo at 6258 Hz")	\
	t(G(2, 0, 128, 1, 1, 1), 4, "DMA GPIP I7 stereo at 6258 Hz")	\
									\
	t(G(2, 0,   8, 5, 3, 4), 4, "DMA GPIP I7 stereo at 6258 Hz")	\
	t(G(2, 0,   9, 7, 2, 3), 4, "DMA GPIP I7 stereo at 6258 Hz")	\
	t(G(2, 0,  10, 3, 4, 2), 4, "DMA GPIP I7 stereo at 6258 Hz")	\
	t(G(2, 0,  11, 4, 5, 6), 4, "DMA GPIP I7 stereo at 6258 Hz")	\
	t(G(2, 0,  32, 2, 7, 3), 4, "DMA GPIP I7 stereo at 6258 Hz")	\
	t(G(2, 0,  64, 6, 2, 4), 4, "DMA GPIP I7 stereo at 6258 Hz")	\
	t(G(2, 0, 128, 4, 6, 7), 4, "DMA GPIP I7 stereo at 6258 Hz")	\
									\
	t(G(2, 1,   8, 1, 1, 1), 4, "DMA GPIP I7 stereo at 12517 Hz")

// FIXME: Needs support for more than 99 subtunes.
//
//	t(G(2, 1,   9, 1, 1, 1), 4, "DMA GPIP I7 stereo at 12517 Hz")
//	t(G(2, 1,  10, 1, 1, 1), 4, "DMA GPIP I7 stereo at 12517 Hz")
//	t(G(2, 1,  11, 1, 1, 1), 4, "DMA GPIP I7 stereo at 12517 Hz")
//	t(G(2, 1,  32, 1, 1, 1), 4, "DMA GPIP I7 stereo at 12517 Hz")
//	t(G(2, 1,  64, 1, 1, 1), 4, "DMA GPIP I7 stereo at 12517 Hz")
//	t(G(2, 1, 128, 1, 1, 1), 4, "DMA GPIP I7 stereo at 12517 Hz")
//
//	t(G(2, 1,   8, 5, 3, 4), 4, "DMA GPIP I7 stereo at 12517 Hz")
//	t(G(2, 1,   9, 7, 2, 3), 4, "DMA GPIP I7 stereo at 12517 Hz")
//	t(G(2, 1,  10, 3, 4, 2), 4, "DMA GPIP I7 stereo at 12517 Hz")
//	t(G(2, 1,  11, 4, 5, 6), 4, "DMA GPIP I7 stereo at 12517 Hz")
//	t(G(2, 1,  32, 2, 7, 3), 4, "DMA GPIP I7 stereo at 12517 Hz")
//	t(G(2, 1,  64, 6, 2, 4), 4, "DMA GPIP I7 stereo at 12517 Hz")
//	t(G(2, 1, 128, 4, 6, 7), 4, "DMA GPIP I7 stereo at 12517 Hz")

#endif /* PSGPLAY_TEST_DMASINT_H */
