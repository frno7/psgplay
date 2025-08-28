// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2025 Fredrik Noring
 */

#ifndef PSGPLAY_TEST_SNDHTIMERVBL_H
#define PSGPLAY_TEST_SNDHTIMERVBL_H

#include "toslibc/asm/machine.h"

#include "internal/macro.h"

#define SNDH_TIMER_FREQUENCY 50

#define TIMER_FREQUENCY	((double)ATARI_STE_PAL_MCLK /			\
	(ATARI_STE_CPU_CLK_DIV * ATARI_STE_CYCLES_PER_VBL_PAL))

#define tune_value_names(t)						\
	t(TIMER_FREQUENCY, "SNDH timer VBL " XSTR(SNDH_TIMER_FREQUENCY) " Hz")

#endif /* PSGPLAY_TEST_SNDHTIMERVBL_H */
