// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2020 Fredrik Noring
 */

#include <stdlib.h>

#include "system/atari/clock.h"

static uint64_t now;

void clock_tick(void)
{
	now += 5;	/* 200 Hz clock tick is 5 ms */
}

uint64_t clock_ms(void)
{
	return now;
}
