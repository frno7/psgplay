// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2025 Fredrik Noring
 */

#ifndef PSGPLAY_TEST_SNDHTIMERC_H
#define PSGPLAY_TEST_SNDHTIMERC_H

#include "internal/macro.h"

#define SNDH_TIMER_FREQUENCY 200

#define tune_value_names(t)						\
	t(SNDH_TIMER_FREQUENCY, "SNDH timer C " XSTR(SNDH_TIMER_FREQUENCY) " Hz")

#endif /* PSGPLAY_TEST_SNDHTIMERC_H */
