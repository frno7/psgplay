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
#include "atari/trace.h"

static void mmu_trace(struct machine *machine,
	const char *op, uint32_t dev_address,
	const char *spacing, int size, uint32_t value,
	size_t (*sh)(struct machine *machine,
		const struct device *device,
		uint32_t dev_address, char *buf, size_t size),
	const struct device *dev)
{
	char description[256];

	if (!machine->trace || machine->trace->m == TRACE_DEVICE_NONE)
		return;

	if (strcmp(dev->name, "dma") == 0 && TRACE_ENABLE(machine->trace, DMA))
		goto trace;
	if (strcmp(dev->name, "psg") == 0 && TRACE_ENABLE(machine->trace, PSG))
		goto trace;
	if (strcmp(dev->name, "snd") == 0 && TRACE_ENABLE(machine->trace, SND))
		goto trace;
	if (strcmp(dev->name, "mfp") == 0 && TRACE_ENABLE(machine->trace, MFP))
		goto trace;
	if (strcmp(dev->name, "ram") == 0 && TRACE_ENABLE(machine->trace, RAM))
		goto trace;
	if (strcmp(dev->name, "rom") == 0 && TRACE_ENABLE(machine->trace, ROM))
		goto trace;
	if (dev->bus.address + dev_address < 2048 && TRACE_ENABLE(machine->trace, ZRO))
		goto trace;

	return;

trace:
	if (sh)
		sh(machine, dev, dev_address, description, sizeof(description));
	else
		description[0] = '\0';

	if (description[0] != '\0')
		fprintf(machine->trace->file,
			"%s %8" PRIu64 "  %6x: %s %s%.*x %s\n",
			dev->name, machine_cycle(machine),
			dev->bus.address + dev_address,
			op, spacing, size, value,
			description);
	else
		fprintf(machine->trace->file,
			"%s %8" PRIu64 "  %6x: %s %s%.*x\n",
			dev->name, machine_cycle(machine),
			dev->bus.address + dev_address,
			op, spacing, size, value);
}

void mmu_trace_rd_u8(struct machine *machine,
	uint32_t dev_address, uint32_t value, const struct device *dev)
{
	mmu_trace(machine, "rd  u8", dev_address, "  ", 2, value, dev->id_u8, dev);
}

void mmu_trace_rd_u16(struct machine *machine,
	uint32_t dev_address, uint32_t value, const struct device *dev)
{
	mmu_trace(machine, "rd u16", dev_address, "", 4, value, dev->id_u16, dev);
}

void mmu_trace_wr_u8(struct machine *machine,
	uint32_t dev_address, uint32_t value, const struct device *dev)
{
	mmu_trace(machine, "wr  u8", dev_address, "  ", 2, value, dev->id_u8, dev);
}

void mmu_trace_wr_u16(struct machine *machine,
	uint32_t dev_address, uint32_t value, const struct device *dev)
{
	mmu_trace(machine, "wr u16", dev_address, "", 4, value, dev->id_u16, dev);
}
