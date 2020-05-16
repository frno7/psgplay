// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#ifndef PSGPLAY_SYSTEM_ATARI_CLOCK_H
#define PSGPLAY_SYSTEM_ATARI_CLOCK_H

#include "internal/types.h"

void clock_tick(void);

u64 clock_ms(void);

#endif /* PSGPLAY_SYSTEM_ATARI_CLOCK_H */
