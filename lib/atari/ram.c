// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#include <stdio.h>
#include <string.h>

#include "internal/compare.h"

#include "atari/device.h"
#include "atari/exception-vector.h"
#include "atari/machine.h"
#include "atari/ram.h"
#include "atari/sound.h"
#include "atari/system-variable.h"

#include "tos/tos.h"

struct ram_map_ro ram_map_ro(struct machine *machine,
	uint32_t size, uint32_t addr)
{
	if (ARRAY_SIZE(machine->ram.u8) <= addr)
		return (struct ram_map_ro) { };

	return (struct ram_map_ro) {
		.size = min_t(uint32_t, size, ARRAY_SIZE(machine->ram.u8) - addr),
		.addr = addr,
		.p = &machine->ram.u8[addr],
	};
}

static void ram_reset(struct machine *machine, const struct device *device)
{
	memcpy(&machine->ram.u8[0], tos, 8);	/* ROM overlay during reset */
	memset(&machine->ram.u8[8], 0, sizeof(machine->ram.u8) - 8);
}

static u8 ram_rd_u8(struct machine *machine, const struct device *device,
	u32 dev_address)
{
	return machine->ram.u8[dev_address];
}

static u16 ram_rd_u16(struct machine *machine, const struct device *device,
	u32 dev_address)
{
	return (machine->ram.u8[dev_address] << 8) |
		machine->ram.u8[dev_address + 1];
}

static void ram_wr_u8(struct machine *machine, const struct device *device,
	u32 dev_address, u8 data)
{
	sound_check(machine, dev_address);

	machine->ram.u8[dev_address] = data;
}

static void ram_wr_u16(struct machine *machine, const struct device *device,
	u32 dev_address, u16 data)
{
	sound_check(machine, dev_address);

	machine->ram.u8[dev_address] = data >> 8;
	machine->ram.u8[dev_address + 1] = data & 0xff;
}

static size_t ram_id_u8(struct machine *machine,
	const struct device *device, u32 dev_address, char *buf, size_t size)
{
	const char *description = exception_vector_description(dev_address);

	if (description[0] == '\0')
		description = system_variable_label(dev_address);

	snprintf(buf, size, "%s", description);

	return strlen(buf);
}

static size_t ram_id_u16(struct machine *machine,
	const struct device *device, u32 dev_address, char *buf, size_t size)
{
	return ram_id_u8(machine, device, dev_address, buf, size);
}

const struct device ram_device = {
	.name = "ram",
	.clk = {
		.frequency = CPU_FREQUENCY,
		.divisor = 1
	},
	.main_bus = true,
	.bus = {
		.address = 0,
		.size = MACHINE_RAM_SIZE,
	},
	.reset  = ram_reset,
	.rd_u8  = ram_rd_u8,
	.rd_u16 = ram_rd_u16,
	.wr_u8  = ram_wr_u8,
	.wr_u16 = ram_wr_u16,
	.id_u8  = ram_id_u8,
	.id_u16 = ram_id_u16,
};
