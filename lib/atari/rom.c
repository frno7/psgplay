// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#include "atari/bus.h"
#include "atari/device.h"
#include "atari/rom.h"

#include "tos/tos.h"

static u8 rom_rd_u8(const struct device *device, u32 dev_address)
{
	return dev_address + 1 <= sizeof(tos) ? tos[dev_address] : 0;
}

static u16 rom_rd_u16(const struct device *device, u32 dev_address)
{
	return dev_address + 2 <= sizeof(tos) ?
		(tos[dev_address] << 8) | tos[dev_address + 1] : 0;
}

const struct device rom0_device = {
	.name = "rom",
	.bus = {
		.address = 0,
		.size = 8,
	},
	.rd_u8  = rom_rd_u8,
	.rd_u16 = rom_rd_u16,
	.wr_u8  = bus_error_wr_u8,
	.wr_u16 = bus_error_wr_u16,
};

const struct device rom1_device = {
	.name = "rom",
	.frequency = 8000000,
	.bus = {
		.address = 0xe00000,
		.size = 256 * 1024,
	},
	.rd_u8  = rom_rd_u8,
	.rd_u16 = rom_rd_u16,
	.wr_u8  = bus_error_wr_u8,
	.wr_u16 = bus_error_wr_u16,
};
