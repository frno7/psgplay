// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2021 Fredrik Noring
 */

#ifndef INTERNAL_BIT_H
#define INTERNAL_BIT_H

#include "internal/types.h"

/**
 * bitrev16 - reverse 16-bit word
 * @x: word to reverse
 *
 * Return: bit-reversed word
 */
uint16_t bitrev16(uint16_t x);

/**
 * bitpop16 - 16-bit word population count
 * @x: word to count
 *
 * Return: number of bits in @x that are 1
 */
uint16_t bitpop16(uint16_t x);

/**
 * bitcompress16 - compress a 16-bit word
 * @x: word to compress
 * @m: compression selection word
 *
 * Example: Let @x = abcd efgh ijkl mnop, and
 *              @m = 0000 1111 0011 1100, then the result is
 *                   0000 0000 efgh klmn.
 *
 * Return: bits in @x chosen by bits in @m
 */
uint16_t bitcompress16(uint16_t x, uint16_t m);

/**
 * bitexpand16 - expand a 16-bit word
 * @x: word to expand
 * @m: expansion selection word
 *
 * Example: Let @x = 0000 0000 efgh klmn, and
 *              @m = 0000 1111 0011 1100, then the result is
 *                   0000 efgh 00kl mn00.
 *
 * Return: bits in @x chosen by bits in @m
 */
uint16_t bitexpand16(uint16_t x, uint16_t m);

/**
 * bitsuccessor16 - successor to a compressed 16-bit word expanded
 * @x: word to add by 1 in compressed form
 * @m: compression and expansion of word
 *
 * Return: word added one in compressed form, and then expanded, with the
 * 	other bits retained
 */
uint16_t bitsuccessor16(uint16_t x, uint16_t m);

#endif /* INTERNAL_BIT_H */
