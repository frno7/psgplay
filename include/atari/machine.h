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
	uint32_t d[8];	/* Data registers */
	uint32_t a[8];	/* Address registers */
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
	DEVICE_LIST_MAX = 16,
};

struct machine {
	void (*init)(struct machine *machine,
		const void *prg, size_t size, size_t offset,
		const struct machine_registers *regs,
		const struct machine_ports *ports);
	bool (*run)(struct machine *machine);

	u64 cycle;

	struct {
		void (*cb)(uint32_t pc, void *arg);
		void *arg;
	} instruction_callback;

	struct {
		struct machine_device_list {
			struct machine_device {
				u64 machine_cycle_event;
				const struct device *device;
			} d[DEVICE_LIST_MAX];
		} list;

		struct device_run_cycle {
			uint64_t machine_slice_end;
		} device_run_cycle;
	} device;

	struct {
		uint32_t irq_pending;

		struct device_cycle vbl_cycle;
	} glue;

	struct {
		struct m68k_module m68k;

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
						uint16_t data;
						uint16_t mask;
					};
					uint16_t halfword[2];
					uint8_t byte[4];
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

#endif /* ATARI_MACHINE_H */
