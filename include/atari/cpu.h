// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#ifndef ATARI_CPU_H
#define ATARI_CPU_H

#include "atari/device.h"

#include "m68k/m68kcpu.h"

void m68k_instruction_callback(struct m68k_module *module, int pc);

void cpu_instruction_callback(struct machine *machine,
	void (*cb)(uint32_t pc, void *arg), void *arg);

u64 cpu_cycles_run(struct machine *machine);

extern const struct device cpu_device;

#endif /* ATARI_CPU_H */
