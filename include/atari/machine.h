// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#ifndef ATARI_MACHINE_H
#define ATARI_MACHINE_H

#include "toslibc/asm/machine.h"

#include "internal/types.h"

#include "atari/sample.h"

#include "cf2149/module/cf2149.h"
#include "cf300588/module/cf300588-sound.h"
#include "cf68901/module/cf68901.h"
#include "m68k/m68kcpu.h"

#define CPU_FREQUENCY (ATARI_STE_PAL_MCLK / ATARI_STE_CPU_CLK_DIV)

#define MACHINE_PROGRAM   0x40000	/* 256 KiB */
#define MACHINE_RUN_SLICE   10000

struct device_cycle {
	u64 c;
};

struct machine_registers {
	u32 d[8];	/* Data registers */
	u32 a[8];	/* Address registers */
};

struct machine_ports {
	psg_sample_f psg_sample;
	sound_sample_f sound_sample;
	mixer_sample_f mixer_sample;
	record_sample_f record_sample;
	void *arg;
};

enum {
	MACHINE_RAM_SIZE = 4 * 1024 * 1024,	/* 4 MiB of RAM */
};

struct machine {
	void (*init)(struct machine *machine,
		const void *prg, size_t size, size_t offset,
		const struct machine_registers *regs,
		const struct machine_ports *ports);
	bool (*run)(struct machine *machine);

	struct {
		void (*cb)(uint32_t pc, void *arg);
		void *arg;
	} instruction_callback;

	struct {
		struct device_cycle vbl_cycle;
	} glue;

	struct {
		bool cpu_execute;
	} cpu;

	struct {
		struct cf68901_module cf68901;

		bool dma_sound_active_prev_level;
	} mfp;

	struct {
		struct mixer_state {
			struct {
				union {
					struct {
						u16 data;
						u16 mask;
					};
					u16 halfword[2];
					u8 byte[4];
				} wr, rd;
				struct device_cycle cycle;
				struct mixer_sample sample;
			} microwire;

			struct mixer_sample sample;
		} state;

		u64 mixer_emit_latest_cycle;

		struct {
			mixer_sample_f sample;
			void *sample_arg;
		} output;
	} mixer;

	struct {
		struct cf2149_module cf2149;

		struct {
			psg_sample_f sample;
			void *sample_arg;
		} output;
	} psg;

	struct {
		uint8_t u8[MACHINE_RAM_SIZE];
	} ram;

	struct {
		struct cf300588_sound_module cf300588;

		struct {
			sound_sample_f sample;
			void *sample_arg;

			record_sample_f record;
			void *record_arg;
		} output;
	} sound;
};

u64 cycle_transform(u64 to_frequency, u64 from_frequency, u64 cycle);

u64 cycle_transform_align(u64 to_frequency, u64 from_frequency, u64 cycle);

u64 machine_cycle(struct machine *machine);

void atari_st_init(struct machine *machine,
	const void *prg, size_t size, size_t offset,
	const struct machine_registers *regs,
	const struct machine_ports *ports);

bool atari_st_run(struct machine *machine);

static inline struct machine *machine_from_m68k_module(
	const struct m68k_module *module)
{
	return module->callback.arg;
}

extern struct m68k_module musashi_module;

#endif /* ATARI_MACHINE_H */
