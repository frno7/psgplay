// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#ifndef ATARI_RAM_H
#define ATARI_RAM_H

#include "atari/bus.h"

struct ram_map_ro {
	size_t size;
	uint32_t addr;
	const void *p;
};

struct ram_map_ro ram_map_ro(uint32_t size, uint32_t addr);

extern const struct device ram_device;

#endif /* ATARI_RAM_H */
