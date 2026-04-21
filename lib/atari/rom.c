// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#include "atari/bus.h"
#include "atari/device.h"
#include "atari/machine.h"
#include "atari/rom.h"

#include "tos/tos.h"

static uint8_t rom_rd_u8(struct machine *machine, const struct device *device,
	uint32_t dev_address)
{
	return dev_address + 1 <= sizeof(tos) ? tos[dev_address] : 0;
}

static uint16_t rom_rd_u16(struct machine *machine, const struct device *device,
	uint32_t dev_address)
{
	return dev_address + 2 <= sizeof(tos) ?
		(tos[dev_address] << 8) | tos[dev_address + 1] : 0;
}

const struct device rom_device = {
	.name = "rom",
	.clk = {
		.frequency = CPU_FREQUENCY,
		.divisor = 1
	},
	.bus = {
		.address = 0xe00000,
		.size = 256 * 1024,
	},
	.rd_u8  = rom_rd_u8,
	.rd_u16 = rom_rd_u16,
	.wr_u8  = bus_error_wr_u8,
	.wr_u16 = bus_error_wr_u16,
};
