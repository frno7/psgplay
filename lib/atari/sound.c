// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2020 Fredrik Noring
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "toslibc/asm/machine.h"

#include "internal/assert.h"
#include "internal/compare.h"
#include "internal/macro.h"

#include "atari/bus.h"
#include "atari/device.h"
#include "atari/irq.h"
#include "atari/glue.h"
#include "atari/machine.h"
#include "atari/mfp.h"
#include "atari/mmu.h"
#include "atari/ram.h"
#include "atari/sound.h"

#include "cf300588/module/cf300588-sound.h"

#define SOUND_FREQUENCY (ATARI_STE_EXT_OSC / 4)

#define SOUND_EVENT_FREQUENCY 100		/* 10 ms */
#define SOUND_EVENT_CYCLES (SOUND_FREQUENCY / SOUND_EVENT_FREQUENCY)

static struct cf300588_sound_module cf300588;

static struct {
	sound_sample_f sample;
	void *sample_arg;

	record_sample_f record;
	void *record_arg;
} output;

static char *sound_register_name(u32 reg)
{
	switch (reg) {
#define SOUND_REG_NAME(register_, symbol_, label_, description_)	\
	case register_: return #symbol_;
CF300588_SOUND_REGS(SOUND_REG_NAME)
	default:
		return "";
	}
}

static void request_event(const struct device *device,
	struct device_cycle sound_cycle, struct cf300588_sound_event event)
{
	if (event.cycle.c)
		request_device_event(device, (struct device_cycle) {
			/* FIXME: Avoid max */
			.c = max(event.cycle.c, sound_cycle.c + 1)
		});

	request_device_event(device, (struct device_cycle) {
			.c = sound_cycle.c + SOUND_EVENT_CYCLES
		});

	for (size_t i = 1; i < event.sint.count; i++)
		dma_sound_active(event.sint.active ^ (i & 1));
	dma_sound_active(event.sint.active);
}

static void sound_event(const struct device *device,
	const struct device_cycle sound_cycle)
{
	struct cf300588_sound_cycle module_cycle =
		cf300588_sound_cycle(sound_cycle.c, 1 /* FIXME */);
	struct cf300588_sound_sample buffer8[1024];

	const struct cf300588_sound_dma_region dma_region =
		cf300588.port.dma(&cf300588);
	const struct ram_map_ro ram_map =
		ram_map_ro(dma_region.size, dma_region.addr);
	const struct cf300588_sound_dma_map dma_map = {
		.size = ram_map.size,
		.addr = ram_map.addr,
		.p = ram_map.p,
	};
	const struct cf300588_sound_samples samples8 = {
		.size = ARRAY_SIZE(buffer8),
		.sample = buffer8,
	};
	size_t n;

	while ((n = cf300588.port.sample(&cf300588,
			module_cycle, samples8, dma_map)))
		if (output.sample) {
			struct sound_sample buffer16[ARRAY_SIZE(buffer8)];

			for (size_t i = 0; i < n; i++)
				buffer16[i] = (struct sound_sample) {
					.left  = 256 * buffer8[i].left,
					.right = 256 * buffer8[i].right
				};

			output.sample(buffer16, n, output.sample_arg);
		}

	request_event(device, sound_cycle,
		cf300588.port.event(&cf300588, module_cycle));
}

static u8 sound_rd_u8(const struct device *device, u32 dev_address)
{
	const struct device_cycle sound_cycle = device_cycle(device);
	struct cf300588_sound_cycle module_cycle =
		cf300588_sound_cycle(sound_cycle.c, 1 /* FIXME */);
	const u32 reg = dev_address >> 1;

	if ((dev_address & 1) == 0)
		return 0;

	sound_event(device, sound_cycle);

	return cf300588.port.rd_da(&cf300588, module_cycle, reg);
}

static u16 sound_rd_u16(const struct device *device, u32 dev_address)
{
	return sound_rd_u8(device, dev_address + 1);
}

static void sound_wr_u8(const struct device *device, u32 dev_address, u8 val)
{
	const struct device_cycle sound_cycle = device_cycle(device);
	struct cf300588_sound_cycle module_cycle =
		cf300588_sound_cycle(sound_cycle.c, 1 /* FIXME */);
	const u32 reg = dev_address >> 1;

	if ((dev_address & 1) == 0)
		return;

	if (reg == 0 && (val & 0x10))
		output.record(sound_cycle.c / 8, output.record_arg);

	sound_event(device, sound_cycle);

	request_event(device, sound_cycle,
		cf300588.port.wr_da(&cf300588, module_cycle, reg, val));
}

static void sound_wr_u16(const struct device *device, u32 dev_address, u16 data)
{
	sound_wr_u8(device, dev_address + 1, data);
}

static size_t sound_id_u8(const struct device *device,
	u32 dev_address, char *buf, size_t size)
{
	const u32 reg = dev_address >> 1;

	if ((dev_address & 1) == 0 || ARRAY_SIZE(cf300588.state.regs.reg) <= reg)
		snprintf(buf, size, "%2u", dev_address);
	else
		snprintf(buf, size, "%s", sound_register_name(reg));

	return strlen(buf);
}

static size_t sound_id_u16(const struct device *device,
	u32 dev_address, char *buf, size_t size)
{
	return sound_id_u8(device, dev_address + 1, buf, size);
}

static void sound_reset(const struct device *device)
{
	const struct device_cycle sound_cycle = device_cycle(device);

	struct cf300588_sound_cycle module_cycle =
		cf300588_sound_cycle(sound_cycle.c, 1 /* FIXME */);

	cf300588 = cf300588_sound_init(module_cycle);

	request_event(device, sound_cycle, (struct cf300588_sound_event) { });
}

void sound_sample(sound_sample_f sample, void *sample_arg)
{
	output.sample = sample;
	output.sample_arg = sample_arg;
}

void record_sample(record_sample_f record, void *record_arg)
{
	output.record = record;
	output.record_arg = record_arg;
}

void sound_check(u32 bus_address)
{
	if (!cf300588.port.wr_ar(&cf300588, bus_address, 4 /* FIXME */))
		return;

	extern const struct device sound_device;
	const struct device *device = &sound_device;

	sound_event(device, device_cycle(device));
}

const struct device sound_device = {
	.name = "snd",
	.clk = {
		.frequency = SOUND_FREQUENCY,
		.divisor = 1
	},
	.bus = {
		.address = 0xff8900,
		.size = 34,
	},
	.reset = sound_reset,
	.event = sound_event,
	.rd_u8  = sound_rd_u8,
	.rd_u16 = sound_rd_u16,
	.wr_u8  = sound_wr_u8,
	.wr_u16 = sound_wr_u16,
	.id_u8  = sound_id_u8,
	.id_u16 = sound_id_u16,
};
