// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#include "internal/build-assert.h"
#include "internal/macro.h"

#include "atari/bus.h"
#include "atari/machine.h"

#include "psgplay/assert.h"

u8 bus_error_rd_u8(const struct device *device, u32 address)
{
	pr_fatal_error("bus error: rd u8 %x\n", device->bus.address + address);

	/* FIXME: m68k_pulse_bus_error(); */
}

u16 bus_error_rd_u16(const struct device *device, u32 address)
{
	pr_fatal_error("bus error: rd u16 %x\n", device->bus.address + address);

	/* FIXME: m68k_pulse_bus_error(); */
}

void bus_error_wr_u8(const struct device *device, u32 address, u8 data)
{
	pr_fatal_error("bus error: wr u8 %x %x\n", device->bus.address + address, data);

	/* FIXME: m68k_pulse_bus_error(); */
}

void bus_error_wr_u16(const struct device *device, u32 address, u16 data)
{
	pr_fatal_error("bus error: wr u16 %x %x\n", device->bus.address + address, data);

	/* FIXME: m68k_pulse_bus_error(); */
}

const struct device bus_device_error = {
	.name = "bus",
	.rd_u8  = bus_error_rd_u8,
	.rd_u16 = bus_error_rd_u16,
	.wr_u8  = bus_error_wr_u8,
	.wr_u16 = bus_error_wr_u16,
};
