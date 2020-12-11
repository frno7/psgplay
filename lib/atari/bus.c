// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#include "internal/assert.h"
#include "internal/build-assert.h"
#include "internal/macro.h"

#include "m68k/m68kcpu.h"

#include "atari/bus.h"
#include "atari/machine.h"

u8 bus_error_rd_u8(const struct device *device, u32 address)
{
	/* FIXME: Properly report pr_error("bus error: rd u8 %x\n", device->bus.address + address); */

	m68k_pulse_bus_error();

	return 0;
}

u16 bus_error_rd_u16(const struct device *device, u32 address)
{
	/* FIXME: Properly report pr_error("bus error: rd u16 %x\n", device->bus.address + address); */

	m68k_pulse_bus_error();

	return 0;
}

void bus_error_wr_u8(const struct device *device, u32 address, u8 data)
{
	/* FIXME: Properly report pr_error("bus error: wr u8 %x %x\n", device->bus.address + address, data); */

	m68k_pulse_bus_error();
}

void bus_error_wr_u16(const struct device *device, u32 address, u16 data)
{
	/* FIXME: Properly report pr_error("bus error: wr u16 %x %x\n", device->bus.address + address, data); */

	m68k_pulse_bus_error();
}

const struct device bus_device_error = {
	.name = "bus",
	.rd_u8  = bus_error_rd_u8,
	.rd_u16 = bus_error_rd_u16,
	.wr_u8  = bus_error_wr_u8,
	.wr_u16 = bus_error_wr_u16,
};
