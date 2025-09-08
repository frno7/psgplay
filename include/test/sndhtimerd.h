// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2025 Fredrik Noring
 */

#ifndef PSGPLAY_TEST_SNDHTIMERD_H
#define PSGPLAY_TEST_SNDHTIMERD_H

#include "internal/macro.h"

#define SNDH_TIMER_FREQUENCY 480

#define tune_value_time_names(t)					\
	t(SNDH_TIMER_FREQUENCY, 63, "SNDH timer D " XSTR(SNDH_TIMER_FREQUENCY) " Hz")

#endif /* PSGPLAY_TEST_SNDHTIMERD_H */
