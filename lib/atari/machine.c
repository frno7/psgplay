// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#include <inttypes.h>
#include <string.h>

#include "internal/assert.h"
#include "internal/build-assert.h"
#include "internal/compare.h"
#include "internal/macro.h"

#include "atari/device.h"
#include "atari/glue.h"
#include "atari/machine.h"
#include "atari/psg.h"
#include "atari/ram.h"
#include "atari/sound.h"

#include "m68k/m68k.h"
#include "m68k/m68kcpu.h"

#define MACHINE_REGISTERS(reg)						\
	reg(0, d, D)							\
	reg(1, d, D)							\
	reg(2, d, D)							\
	reg(3, d, D)							\
	reg(4, d, D)							\
	reg(5, d, D)							\
	reg(6, d, D)							\
	reg(7, d, D)							\
	reg(0, a, A)							\
	reg(1, a, A)							\
	reg(2, a, A)							\
	reg(3, a, A)							\
	reg(4, a, A)							\
	reg(5, a, A)							\
	reg(6, a, A)							\
	reg(7, a, A)

static u64 cycle;

u64 cycle_transform(u64 to_frequency, u64 from_frequcy, u64 cycle)
{
	const u64 q = cycle / from_frequcy;
	const u64 r = cycle % from_frequcy;

	return q * to_frequency + (r * to_frequency) / from_frequcy;
}

u64 cycle_transform_align(u64 to_frequency, u64 from_frequcy, u64 cycle)
{
	const u64 q = cycle / from_frequcy;
	const u64 r = cycle % from_frequcy;

	return q * to_frequency +
		(r * to_frequency + from_frequcy - 1) / from_frequcy;
}

u64 machine_cycle(void)
{
	const int cycles_run = m68k_cycles_run();

	BUG_ON(cycles_run < 0);

	return cycle + cycles_run;
}

static void atari_st_init(const void *prg, size_t size, size_t offset,
	const struct machine_registers *regs,
	const struct machine_ports *ports)
{
	const u8 *p = prg;

	cycle = 0;
	m68k_clear_timeslice();

	if (offset + size >= ram_device.bus.size)
		pr_fatal_error("Program at %zu bytes too large for %u bytes\n",
			size, ram_device.bus.size);

	device_reset();

#define MACHINE_REGISTER_SET(index_, field_, label_)			\
	m68k_set_reg(M68K_REG_##label_##index_, regs->field_[index_]);
	MACHINE_REGISTERS(MACHINE_REGISTER_SET)

	for (size_t i = 0; i < size; i++)
		ram_device.wr_u8(&ram_device, offset + i, p[i]);

	psg_sample(ports->psg_sample, ports->arg);
	sound_sample(ports->sound_sample, ports->arg);
}

static bool atari_st_run(void)
{
	cycle += device_run(cycle, MACHINE_RUN_SLICE);

	return true;
}

const struct machine atari_st = {
	.init = atari_st_init,
	.run = atari_st_run,
};
