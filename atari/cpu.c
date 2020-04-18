// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 *
 * FIXME: Supervisor address access
 */

#include "atari/cpu.h"
#include "atari/device.h"

#include "m68k/m68k.h"
#include "m68k/m68kcpu.h"

#include "psgplay/assert.h"

void m68k_instruction_callback(int pc)
{
#if 0	/* FIXME */
	printf("%08x: %04x %s\n",
		ADDRESS_68K(REG_PPC), REG_IR,
		m68ki_disassemble_quick(ADDRESS_68K(REG_PPC), M68K_CPU_TYPE_68000));
#endif
}

static struct device_slice cpu_run(const struct device *device,
	struct device_cycle device_cycle, struct device_slice device_slice)
{
	const int s = m68k_execute(device_slice.s);

	BUG_ON(s < 0);

	return (struct device_slice) { .s = s };
}

static void cpu_reset(const struct device *device)
{
	m68k_init();
	m68k_set_cpu_type(M68K_CPU_TYPE_68000);
	m68k_pulse_reset();
}

const struct device cpu_device = {
	.name = "cpu",
	.frequency = 8000000,
	.run = cpu_run,
	.reset = cpu_reset,
};
