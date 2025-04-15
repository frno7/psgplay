// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#include <stdio.h>
#include <string.h>

#include "atari/device.h"
#include "atari/exception-vector.h"
#include "atari/ram.h"
#include "atari/sound.h"
#include "atari/system-variable.h"

#include "tos/tos.h"

static u8 ram[4 * 1024 * 1024];	/* 4 MiB of RAM */

static void ram_reset(const struct device *device)
{
	memcpy(&ram[0], tos, 8);	/* ROM overlay during reset */
	memset(&ram[8], 0, sizeof(ram) - 8);
}

static u8 ram_rd_u8(const struct device *device, u32 dev_address)
{
	return ram[dev_address];
}

static u16 ram_rd_u16(const struct device *device, u32 dev_address)
{
	return (ram[dev_address] << 8) | ram[dev_address + 1];
}

static void ram_wr_u8(const struct device *device,
	u32 dev_address, u8 data)
{
	sound_check(dev_address);

	ram[dev_address] = data;
}

static void ram_wr_u16(const struct device *device,
	u32 dev_address, u16 data)
{
	sound_check(dev_address);

	ram[dev_address] = data >> 8;
	ram[dev_address + 1] = data & 0xff;
}

static size_t ram_id_u8(const struct device *device,
	u32 dev_address, char *buf, size_t size)
{
	const char *description = exception_vector_description(dev_address);

	if (description[0] == '\0')
		description = system_variable_label(dev_address);

	snprintf(buf, size, "%s", description);

	return strlen(buf);
}

static size_t ram_id_u16(const struct device *device,
	u32 dev_address, char *buf, size_t size)
{
	return ram_id_u8(device, dev_address, buf, size);
}

const struct device ram_device = {
	.name = "ram",
	.frequency = 8000000,
	.main_bus = true,
	.bus = {
		.address = 0,
		.size = sizeof(ram),
	},
	.reset  = ram_reset,
	.rd_u8  = ram_rd_u8,
	.rd_u16 = ram_rd_u16,
	.wr_u8  = ram_wr_u8,
	.wr_u16 = ram_wr_u16,
	.id_u8  = ram_id_u8,
	.id_u16 = ram_id_u16,
};
