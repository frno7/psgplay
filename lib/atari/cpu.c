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

void m68k_instruction_callback(struct m68k_module *module, int pc)
{
	struct machine *machine = machine_from_m68k_module(module);

#if 0	/* FIXME */
	printf("%08x: %04x %s\n", pc, REG_IR,
		m68ki_disassemble_quick(pc, M68K_CPU_TYPE_68000));
#endif

	if (!machine->instruction_callback.cb)
		return;

	machine->instruction_callback.cb(pc, machine->instruction_callback.arg);
}

void cpu_instruction_callback(struct machine *machine,
	void (*cb)(uint32_t pc, void *arg), void *arg)
{
	machine->instruction_callback.cb = cb;
	machine->instruction_callback.arg = arg;
}

uint64_t cpu_cycles_run(struct machine *machine)
{
	const int cycles_run = machine->cpu.cpu_execute ?
		m68k_cycles_run(&machine->cpu.m68k) : 0;

#if 0  /* FIXME: Dependency on pr_bug */
	BUG_ON(cycles_run < 0);
#endif

	return cycles_run;
}

static struct device_slice cpu_run(struct machine *machine,
	const struct device *device, struct device_cycle device_cycle,
	struct device_slice device_slice)
{
	machine->cpu.cpu_execute = true;
	const int s = m68k_execute(&machine->cpu.m68k, device_slice.s);
	machine->cpu.cpu_execute = false;

#if 0  /* FIXME: Dependency on pr_bug */
	BUG_ON(s < 0);
#endif

	return (struct device_slice) { .s = s };
}

static void cpu_reset(struct machine *machine, const struct device *device)
{
	machine->instruction_callback.cb = NULL;
	machine->instruction_callback.arg = NULL;

	m68k_init(&machine->cpu.m68k);
	m68k_set_callback_arg(&machine->cpu.m68k, machine);
	m68k_set_cpu_type(&machine->cpu.m68k, M68K_CPU_TYPE_68000);
	m68k_pulse_reset(&machine->cpu.m68k);
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
