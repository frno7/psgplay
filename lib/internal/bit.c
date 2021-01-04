// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2021 Fredrik Noring
 */

#include "internal/bit.h"

uint16_t bitrev16(uint16_t x)
{
	uint16_t r = 0;

	for (int i = 0; i < 16; i++, x >>= 1)
		r |= (x & 1) << (15 - i);

	return r;
}

uint16_t bitpop16(uint16_t x)
{
	uint16_t n = 0;

	while (x) {
		x &= x - 1;
		n++;
	}

	return n;
}
