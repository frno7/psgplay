// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#include "internal/build-assert.h"
#include "internal/macro.h"

#include "m68k/m68kcpu.h"

#include "atari/bus.h"
#include "atari/machine.h"

uint8_t bus_error_rd_u8(struct machine *machine,const struct device *device,
	u32 address)
{
	/* FIXME: Properly report pr_error("bus error: rd u8 %x\n", device->bus.address + address); */

	m68k_pulse_bus_error(&machine->cpu.m68k);

	return 0;
}

uint16_t bus_error_rd_u16(struct machine *machine, const struct device *device,
	u32 address)
{
	/* FIXME: Properly report pr_error("bus error: rd u16 %x\n", device->bus.address + address); */

	m68k_pulse_bus_error(&machine->cpu.m68k);

	return 0;
}

void bus_error_wr_u8(struct machine *machine, const struct device *device,
	u32 address, uint8_t data)
{
	/* FIXME: Properly report pr_error("bus error: wr u8 %x %x\n", device->bus.address + address, data); */

	m68k_pulse_bus_error(&machine->cpu.m68k);
}

void bus_error_wr_u16(struct machine *machine, const struct device *device,
	u32 address, uint16_t data)
{
	/* FIXME: Properly report pr_error("bus error: wr u16 %x %x\n", device->bus.address + address, data); */

	m68k_pulse_bus_error(&machine->cpu.m68k);
}

const struct device bus_device_error = {
	.name = "bus",
	.rd_u8  = bus_error_rd_u8,
	.rd_u16 = bus_error_rd_u16,
	.wr_u8  = bus_error_wr_u8,
	.wr_u16 = bus_error_wr_u16,
};
