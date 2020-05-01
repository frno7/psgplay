// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#include <inttypes.h>
#include <string.h>

#include "internal/build-assert.h"

#include "atari/device.h"
#include "atari/glue.h"
#include "atari/machine.h"
#include "atari/psg.h"
#include "atari/ram.h"

#include "m68k/m68k.h"
#include "m68k/m68kcpu.h"

#include "psgplay/assert.h"
#include "psgplay/compare.h"
#include "psgplay/macro.h"

static u64 cycle = 0;

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

static void atari_st_init(const void *prg, size_t size, u32 track,
	u32 timer, int sample_frequency, sample_f sample, void *sample_arg)
{
	const size_t offset = 0x4000;	/* 16 KiB */
	const u8 *p = prg;

	if (offset + size >= ram_device.bus.size)
		pr_fatal_error("Program at %zu bytes too large for %u bytes\n",
			size, ram_device.bus.size);

	device_reset();

	m68k_set_reg(M68K_REG_D0, size);
	m68k_set_reg(M68K_REG_A0, offset);
	m68k_set_reg(M68K_REG_D1, track);
	m68k_set_reg(M68K_REG_D2, timer);

	for (size_t i = 0; i < size; i++)
		ram_device.wr_u8(&ram_device, offset + i, p[i]);

	psg_sample(sample_frequency, sample, sample_arg);
}

static bool atari_st_run(void)
{
	cycle += device_run(cycle, 10000);

	return !psg_output_halt();
}

const struct machine atari_st = {
	.init = atari_st_init,
	.run = atari_st_run,
};
