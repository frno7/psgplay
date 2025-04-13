// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 *
 * FIXME: Supervisor address access
 */

#include "internal/assert.h"

#include "atari/cpu.h"
#include "atari/device.h"

#include "m68k/m68k.h"
#include "m68k/m68kcpu.h"

static struct {
	void (*cb)(uint32_t pc, void *arg);
	void *arg;
} instruction_callback;

void m68k_instruction_callback(int pc)
{
#if 0	/* FIXME */
	printf("%08x: %04x %s\n", pc, REG_IR,
		m68ki_disassemble_quick(pc, M68K_CPU_TYPE_68000));
#endif

	if (!instruction_callback.cb)
		return;

	instruction_callback.cb(pc, instruction_callback.arg);
}

void cpu_instruction_callback(void (*cb)(uint32_t pc, void *arg), void *arg)
{
	instruction_callback.cb = cb;
	instruction_callback.arg = arg;
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
	instruction_callback.cb = NULL;
	instruction_callback.arg = NULL;

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
