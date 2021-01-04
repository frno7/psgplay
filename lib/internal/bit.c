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

uint16_t bitcompress16(uint16_t x, uint16_t m)
{
	uint16_t r = 0;

	for (uint16_t s = 0; m; x >>= 1, m >>= 1) {
		const uint16_t b = m & 1;

		r |= (x & b) << s;
		s += b;
	}

	return r;
}

uint16_t bitexpand16(uint16_t x, uint16_t m)
{
	uint16_t r = 0;

	for (uint16_t s = 0; m; s++, m >>= 1) {
		const uint16_t b = m & 1;

		r |= (x & b) << s;
		x >>= b;
	}

	return r;
}

uint16_t bitsuccessor16(uint16_t x, uint16_t m)
{
	return bitexpand16(bitcompress16(x, m) + 1, m) | (x & ~m);
}
