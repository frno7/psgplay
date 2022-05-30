// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#ifndef ATARI_MACHINE_H
#define ATARI_MACHINE_H

#include "internal/types.h"

#include "atari/sample.h"

#define MACHINE_PROGRAM   0x40000	/* 256 KiB */
#define MACHINE_FREQUENCY 8000000
#define MACHINE_RUN_SLICE   10000

struct machine_registers {
	u32 d[8];	/* Data registers */
	u32 a[8];	/* Address registers */
};

struct machine_ports {
	psg_sample_f psg_sample;
	sound_sample_f sound_sample;
	mixer_sample_f mixer_sample;
	void *arg;
};

struct machine {
	void (*init)(const void *prg, size_t size, size_t offset,
		const struct machine_registers *regs,
		const struct machine_ports *ports);
	bool (*run)(void);
};

u64 cycle_transform(u64 to_frequency, u64 from_frequcy, u64 cycle);

u64 cycle_transform_align(u64 to_frequency, u64 from_frequcy, u64 cycle);

u64 machine_cycle(void);

void machine_execute(void);

extern const struct machine atari_st;

#endif /* ATARI_MACHINE_H */
