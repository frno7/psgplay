// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 *
 * FIXME: Supervisor address access
 */

#include "atari/cpu.h"
#include "atari/device.h"
#include "atari/machine.h"

#include "m68k/m68k.h"
#include "m68k/m68kcpu.h"

static struct {
	void (*cb)(uint32_t pc, void *arg);
	void *arg;
} instruction_callback;

bool cpu_execute;

void m68k_instruction_callback(struct m68k_module *module, int pc)
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

u64 cpu_cycles_run(void)
{
	const int cycles_run = cpu_execute ?
		m68k_cycles_run(&musashi_module) : 0;

#if 0  /* FIXME: Dependency on pr_bug */
	BUG_ON(cycles_run < 0);
#endif

	return cycles_run;
}

static struct device_slice cpu_run(const struct device *device,
	struct device_cycle device_cycle, struct device_slice device_slice)
{
	cpu_execute = true;
	const int s = m68k_execute(&musashi_module, device_slice.s);
	cpu_execute = false;

#if 0  /* FIXME: Dependency on pr_bug */
	BUG_ON(s < 0);
#endif

	return (struct device_slice) { .s = s };
}

static void cpu_reset(const struct device *device)
{
	instruction_callback.cb = NULL;
	instruction_callback.arg = NULL;

	m68k_init(&musashi_module);
	m68k_set_cpu_type(&musashi_module, M68K_CPU_TYPE_68000);
	m68k_pulse_reset(&musashi_module);
}

const struct device cpu_device = {
	.name = "cpu",
	.clk = {
		.frequency = CPU_FREQUENCY,
		.divisor = 1
	},
	.run = cpu_run,
	.reset = cpu_reset,
};
