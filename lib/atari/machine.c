// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#include <inttypes.h>
#include <string.h>

#include "internal/build-assert.h"
#include "internal/compare.h"
#include "internal/macro.h"

#include "atari/cpu.h"
#include "atari/device.h"
#include "atari/glue.h"
#include "atari/machine.h"
#include "atari/mixer.h"
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

uint64_t cycle_transform(uint64_t to_frequency, uint64_t from_frequency, uint64_t cycle)
{
	const uint64_t q = cycle / from_frequency;
	const uint64_t r = cycle % from_frequency;

	return q * to_frequency + (r * to_frequency) / from_frequency;
}

uint64_t cycle_transform_align(uint64_t to_frequency, uint64_t from_frequency, uint64_t cycle)
{
	const uint64_t q = cycle / from_frequency;
	const uint64_t r = cycle % from_frequency;

	return q * to_frequency +
		(r * to_frequency + from_frequency - 1) / from_frequency;
}

uint64_t machine_cycle(struct machine *machine)
{
	return machine->cycle + cpu_cycles_run(machine);
}

void atari_st_init(struct machine *machine,
	const void *prg, size_t size, size_t offset,
	const struct machine_registers *regs,
	const struct machine_ports *ports)
{
	const uint8_t *p = prg;

	machine->cycle = 0;
	m68k_clear_timeslice(&machine->cpu.m68k);

#if 0  /* FIXME: Dependency on pr_bug */
	if (offset + size >= ram_device.bus.size)
		pr_fatal_error("Program at %zu bytes too large for %u bytes\n",
			size, ram_device.bus.size);
#endif

	device_reset(machine);

#define MACHINE_REGISTER_SET(index_, field_, label_)			\
	m68k_set_reg(&machine->cpu.m68k, M68K_REG_##label_##index_, regs->field_[index_]);
	MACHINE_REGISTERS(MACHINE_REGISTER_SET)

	for (size_t i = 0; i < size; i++)
		ram_device.wr_u8(machine, &ram_device, offset + i, p[i]);

	psg_sample(machine, ports->psg_sample, ports->arg);
	sound_sample(machine, ports->sound_sample, ports->arg);
	mixer_sample(machine, ports->mixer_sample, ports->arg);
	record_sample(machine, ports->record_sample, ports->arg);
}

bool atari_st_run(struct machine *machine)
{
	machine->cycle += device_run(machine, machine->cycle, MACHINE_RUN_SLICE);

	return true;
}
