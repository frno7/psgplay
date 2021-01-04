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

#endif /* INTERNAL_BIT_H */
