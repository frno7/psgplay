// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#include "internal/assert.h"
#include "internal/types.h"

#include "atari/bus.h"
#include "atari/mfp.h"
#include "atari/machine.h"
#include "atari/mmu.h"
#include "atari/mmu-trace.h"
#include "atari/ram.h"
#include "atari/rom.h"

#include "m68k/m68k.h"
#include "m68k/m68kcpu.h"

static void mmu_bus_wait(const struct device *dev)
{
	if (!dev->main_bus)
		return;

	/*
	 * The MMU interleaves processor accesses to the main bus
	 * every fourth cycle.
	 */

	const u64 mc = machine_cycle();
	const unsigned wait_cycles = ALIGN(mc, 4) - mc;

	if (wait_cycles)
		USE_CYCLES(wait_cycles);
}

#define DMA_DEVICE(bus_address, dev)					\
	valid_device_bus_address(bus_address, dev) ? dev
#define DMA_DEVICES(bus_address)					\
	DMA_DEVICE(bus_address, &ram_device) :				\
	DMA_DEVICE(bus_address, &rom0_device) :				\
	DMA_DEVICE(bus_address, &rom1_device) : NULL

u8 dma_read_memory_8(u32 bus_address)
{
	const struct device *dev = DMA_DEVICES(bus_address);

	if (!dev)
		return 0;

	const u32 dev_address = bus_address - dev->bus.address;
	const u8 value = dev->rd_u8(dev, dev_address);

	mmu_trace_rd_u8(dev_address, value, dev);

	return value;
}

u32 m68k_read_memory_8(u32 bus_address)
{
	const struct device *dev = device_for_bus_address(bus_address);
	const u32 dev_address = bus_address - dev->bus.address;

	mmu_bus_wait(dev);

	const u8 value = dev->rd_u8(dev, dev_address);

	mmu_trace_rd_u8(dev_address, value, dev);

	return value;
}

u32 m68k_read_memory_16(u32 bus_address)
{
	const struct device *dev = device_for_bus_address(bus_address);
	const u32 dev_address = bus_address - dev->bus.address;
	const u16 value = dev->rd_u16(dev, dev_address);

	mmu_bus_wait(dev);

	mmu_trace_rd_u16(dev_address, value, dev);

	return value;
}

u32 m68k_read_memory_32(u32 bus_address)
{
	const u32 hi = m68k_read_memory_16(bus_address);
	const u16 lo = m68k_read_memory_16(bus_address + 2);

	return (hi << 16) | lo;
}

void m68k_write_memory_8(u32 bus_address, u32 value)
{
	const struct device *dev = device_for_bus_address(bus_address);
	const u32 dev_address = bus_address - dev->bus.address;

	mmu_bus_wait(dev);

	mmu_trace_wr_u8(dev_address, value, dev);

	dev->wr_u8(dev, dev_address, value & 0xff);
}

void m68k_write_memory_16(u32 bus_address, u32 value)
{
	const struct device *dev = device_for_bus_address(bus_address);
	const u32 dev_address = bus_address - dev->bus.address;

	mmu_bus_wait(dev);

	mmu_trace_wr_u16(dev_address, value, dev);

	dev->wr_u16(dev, dev_address, value & 0xffff);
}

void m68k_write_memory_32(u32 bus_address, u32 value)
{
	m68k_write_memory_16(bus_address, value >> 16);
	m68k_write_memory_16(bus_address + 2, value & 0xffff);
}

u32 m68k_read_disassembler_16(u32 bus_address)
{
	const struct device *dev = device_for_bus_address(bus_address);
	const u32 dev_address = bus_address - dev->bus.address;

	return dev->rd_u16(dev, dev_address);
}

u32 m68k_read_disassembler_32(u32 bus_address)
{
	const u32 hi = m68k_read_disassembler_16(bus_address);
	const u16 lo = m68k_read_disassembler_16(bus_address + 2);

	return (hi << 16) | lo;
}
