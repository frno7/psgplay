// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2025 Fredrik Noring
 */

#ifndef PSGPLAY_TEST_SNDHTIMERB_H
#define PSGPLAY_TEST_SNDHTIMERB_H

#include "internal/macro.h"

#define SNDH_TIMER_FREQUENCY 75

#define tune_value_names(t)						\
	t(SNDH_TIMER_FREQUENCY, "SNDH timer B " XSTR(SNDH_TIMER_FREQUENCY) " Hz")

#endif /* PSGPLAY_TEST_SNDHTIMERB_H */
