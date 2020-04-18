// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#ifndef PSGPLAY_ROM_H
#define PSGPLAY_ROM_H

#include "psgplay/types.h"

struct rom {
	size_t size;
	u8 *data;
};

struct rom tos_rom(void);

#endif /* PSGPLAY_ROM_H */
