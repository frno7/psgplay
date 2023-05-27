// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2020 Fredrik Noring
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "internal/assert.h"
#include "internal/macro.h"

#include "atari/bus.h"
#include "atari/device.h"
#include "atari/irq.h"
#include "atari/glue.h"
#include "atari/machine.h"
#include "atari/mfp.h"
#include "atari/mmu.h"
#include "atari/sound.h"

#define SOUND_EVENT_FREQUENCY 100		/* 10 ms */
#define SOUND_EVENT_CYCLES (SOUND_FREQUENCY / SOUND_EVENT_FREQUENCY)
#define DEVICE_SAMPLE_FREQUENCY 250000	/* 250 kHz */
#define DEVICE_SAMPLE_CYCLES (SOUND_FREQUENCY / DEVICE_SAMPLE_FREQUENCY)

static union sound sound;
struct {
	u32 start;
	u32 end;

	struct sound_ctrl ctrl;
	struct sound_mode mode;

	struct sound_sample sample;

	struct fifo {
		uint8_t index;
		uint8_t size;
		int8_t b[8];
	} fifo;
} state;

static struct {
	sound_sample_f sample;
	void *sample_arg;
} output;

static char *sound_register_name(u32 reg)
{
	switch (reg) {
#define SOUND_REG_NAME(register_, symbol_, label_, description_)	\
	case register_: return #symbol_;
SOUND_REGISTERS(SOUND_REG_NAME)
	default:
		return "";
	}
}

static u32 sound_frequency(struct sound_mode mode)
{
	switch (mode.sample_frequency)
	{
#define SOUND_SAMPLE_FREQUENCY_CASE(f)					\
	case sound_sample_frequency_##f: return f;
SOUND_SAMPLE_FREQUENCY(SOUND_SAMPLE_FREQUENCY_CASE)
	default:
		BUG();
	}
}

static void sound_start(void)
{
	dma_sound_active(false);

	state.ctrl = sound.ctrl;
	state.start = (sound.starthi << 16) |
		      (sound.startmi <<  8) |
		       sound.startlo;
	state.end = (sound.endhi << 16) |
		    (sound.endmi <<  8) |
		     sound.endlo;
	state.mode = sound.mode;

	dma_sound_active(true);
}

static void sound_stop(void)
{
	memset(&state, 0, sizeof(state));

	dma_sound_active(false);
}

static bool sound_sample_emit(const struct device_cycle sound_cycle)
{
	const u32 f = sound_frequency(state.mode);
	const u64 c = sound_cycle.c;

	return cycle_transform(f, SOUND_FREQUENCY, c) !=
	       cycle_transform(f, SOUND_FREQUENCY, c + DEVICE_SAMPLE_CYCLES);
}

static size_t fifo_free(struct fifo *fifo)
{
	return ARRAY_SIZE(fifo->b) - fifo->size;
}

static void wr_fifo(struct fifo *fifo, int8_t s)
{
	BUG_ON(fifo->size >= ARRAY_SIZE(fifo->b));

	fifo->b[(fifo->index + fifo->size++) % ARRAY_SIZE(fifo->b)] = s;
}

static int8_t rd_fifo(struct fifo *fifo)
{
	BUG_ON(!fifo->size);

	const int8_t s = fifo->b[fifo->index];

	fifo->index = (fifo->index + 1) % ARRAY_SIZE(fifo->b);
	fifo->size--;

	return s;
}

static void fill_fifo(struct fifo *fifo)
{
	while (fifo_free(fifo) >= 2) {
		const uint16_t w = dma_read_memory_16(state.start);

		wr_fifo(fifo, w >> 8);
		wr_fifo(fifo, w);

		state.start += 2;

		if (state.start == state.end)
			(sound.ctrl.loop ? sound_start : sound_stop)();
	}
}

static struct sound_sample sound_sample_cycle(
	const struct device_cycle sound_cycle)
{
	if (state.ctrl.dma)
		fill_fifo(&state.fifo);

	if (state.fifo.size && sound_sample_emit(sound_cycle)) {
		if (state.mode.mono) {
			const s8 mono = rd_fifo(&state.fifo);

			state.sample.left  = 256 * mono;
			state.sample.right = 256 * mono;
		} else {
			state.sample.left  = 256 * rd_fifo(&state.fifo);
			state.sample.right = 256 * (state.fifo.size ?
				rd_fifo(&state.fifo) : state.sample.left);
		}
	}

	return state.sample;
}

static u64 sound_emit_latest_cycle;
static void sound_emit(const struct device_cycle sound_cycle)
{
	struct sound_sample buffer[256];
	size_t count = 0;

	for (u64 c = ALIGN(sound_emit_latest_cycle, DEVICE_SAMPLE_CYCLES);
	     c < sound_cycle.c;
	     c += DEVICE_SAMPLE_CYCLES) {
		buffer[count++] = sound_sample_cycle(
			(struct device_cycle) { .c = c });

		if (count >= ARRAY_SIZE(buffer)) {
			if (output.sample)
				output.sample(buffer, count, output.sample_arg);
			count = 0;
		}
	}

	if (count && output.sample)
		output.sample(buffer, count, output.sample_arg);

	sound.counthi = (state.start >> 16) & 0x3f;
	sound.countmi = state.start >> 8;
	sound.countlo = state.start & 0xfe;

	sound_emit_latest_cycle = sound_cycle.c;
}

static u64 completion_cycles(const u32 sample_count)
{
	const u32 f = sound_frequency(state.mode);
	const u64 ds = cycle_transform_align(
		DEVICE_SAMPLE_FREQUENCY, f, sample_count);
	const u64 sc = cycle_transform_align(
		SOUND_FREQUENCY, DEVICE_SAMPLE_FREQUENCY, ds);

	return sc;
}

static void sound_event(const struct device *device,
	const struct device_cycle sound_cycle)
{
	sound_emit(sound_cycle);

	if (state.ctrl.dma) {
		const size_t m = state.mode.mono ? 1 : 2;
		const size_t remaining = (state.end - state.start) / m;
		const size_t margin = ARRAY_SIZE(state.fifo.b) / m;

		/*
		 * Issue DMA completion event when the FIFO is expected
		 * to read the last sample of this transmission.
		 */

		if (margin < remaining)
			request_device_event(device, (struct device_cycle) {
				.c = sound_cycle.c +
					completion_cycles(remaining - margin)
			});
	}

	request_device_event(device, (struct device_cycle) {
			.c = sound_cycle.c + SOUND_EVENT_CYCLES
		});
}

static u8 sound_rd_u8(const struct device *device, u32 dev_address)
{
	const struct device_cycle sound_cycle = device_cycle(device);
	const u32 reg = dev_address >> 1;

	sound_emit(sound_cycle);

	if ((dev_address & 1) == 0 || ARRAY_SIZE(sound.reg) <= reg)
		return 0;

	return sound.reg[reg];
}

static u16 sound_rd_u16(const struct device *device, u32 dev_address)
{
	return sound_rd_u8(device, dev_address + 1);
}

static void sound_hardwire(void)
{
	sound.reg[SOUND_REG_CTRL]    &= 0x03;
	sound.reg[SOUND_REG_STARTHI] &= 0x3f;
	sound.reg[SOUND_REG_STARTLO] &= 0xfe;
	sound.reg[SOUND_REG_ENDHI]   &= 0x3f;
	sound.reg[SOUND_REG_ENDLO]   &= 0xfe;
	sound.reg[SOUND_REG_MODE]    &= 0x83;

	memset(sound.unused, 0, sizeof(sound.unused));
}

static void sound_wr_u8(const struct device *device, u32 dev_address, u8 data)
{
	const struct device_cycle sound_cycle = device_cycle(device);
	const u32 reg = dev_address >> 1;
	const struct sound_ctrl ctrl = sound.ctrl;

	sound_emit(sound_cycle);

	if ((dev_address & 1) == 0 || ARRAY_SIZE(sound.reg) <= reg)
		return;

	sound.reg[reg] = data;
	sound_hardwire();

	if (ctrl.dma != sound.ctrl.dma)
		(sound.ctrl.dma ? sound_start : sound_stop)();
}

static void sound_wr_u16(const struct device *device, u32 dev_address, u16 data)
{
	sound_wr_u8(device, dev_address + 1, data);
}

static size_t sound_id_u8(const struct device *device,
	u32 dev_address, char *buf, size_t size)
{
	const u32 reg = dev_address >> 1;

	if ((dev_address & 1) == 0 || ARRAY_SIZE(sound.reg) <= reg)
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
	BUILD_BUG_ON(sizeof(sound) != 17);

	memset(&sound, 0, sizeof(sound));
	memset(&state, 0, sizeof(state));
	sound_hardwire();

	sound_emit_latest_cycle = 0;

	sound_event(device, device_cycle(device));
}

void sound_sample(sound_sample_f sample, void *sample_arg)
{
	output.sample = sample;
	output.sample_arg = sample_arg;
}

const struct device sound_device = {
	.name = "snd",
	.frequency = SOUND_FREQUENCY,
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
