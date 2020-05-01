// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#include <inttypes.h>
#include <stdio.h>
#include <string.h>

#include "internal/types.h"

#include "atari/bus.h"
#include "atari/machine.h"

static void mmu_trace(const char *op, u32 dev_address,
	const char *spacing, int size, u32 value,
	size_t (*sh)(const struct device *device,
		u32 dev_address, char *buf, size_t size),
	const struct device *dev)
{
	char description[256];

	return;

	if (strcmp(dev->name, "dma") == 0)
		goto trace;
	if (strcmp(dev->name, "mfp") == 0)
		goto trace;
	if (strcmp(dev->name, "psg") == 0)
		goto trace;
	if (dev->bus.address + dev_address < 2048)
		goto trace;

trace:

	if (sh)
		sh(dev, dev_address, description, sizeof(description));
	else
		description[0] = '\0';

	if (description[0] != '\0')
		printf("%9" PRIu64 " mmu %s %s %x %s%.*x %s\n",
			machine_cycle(), op, dev->name,
			dev->bus.address + dev_address,
			spacing, size, value,
			description);
	else
		printf("%9" PRIu64 " mmu %s %s %x %s%.*x\n",
			machine_cycle(), op, dev->name,
			dev->bus.address + dev_address,
			spacing, size, value);
}

void mmu_trace_rd_u8(u32 dev_address, u32 value, const struct device *dev)
{
	mmu_trace("rd  u8", dev_address, "  ", 2, value, dev->id_u8, dev);
}

void mmu_trace_rd_u16(u32 dev_address, u32 value, const struct device *dev)
{
	mmu_trace("rd u16", dev_address, "", 4, value, dev->id_u16, dev);
}

void mmu_trace_wr_u8(u32 dev_address, u32 value, const struct device *dev)
{
	mmu_trace("wr  u8", dev_address, "  ", 2, value, dev->id_u8, dev);
}

void mmu_trace_wr_u16(u32 dev_address, u32 value, const struct device *dev)
{
	mmu_trace("wr u16", dev_address, "", 4, value, dev->id_u16, dev);
}
