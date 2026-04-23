// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2020 Fredrik Noring
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "internal/compare.h"
#include "internal/macro.h"

#include "atari/bus.h"
#include "atari/device.h"
#include "atari/irq.h"
#include "atari/glue.h"
#include "atari/machine.h"
#include "atari/mmu.h"
#include "atari/mixer.h"

#define MIXER_FREQUENCY 1000000		/* FIXME: What device frequency? */
#define MIXER_EVENT_FREQUENCY 100		/* 10 ms */
#define MIXER_EVENT_CYCLES (MIXER_FREQUENCY / MIXER_EVENT_FREQUENCY)
#define MIXER_SAMPLE_CYCLES 4			/* 250.332 kHz */

static char *mixer_register_name(u32 reg)
{
	switch (reg) {
#define MIXER_MICROWIRE_REG_NAME(register_, symbol_, label_, description_)\
	case register_: return #symbol_;
MIXER_MICROWIRE_REGISTERS(MIXER_MICROWIRE_REG_NAME)
	default:
		return "";
	}
}

static u32 microwire_shift(struct machine *machine,
	const struct device_cycle mixer_cycle)
{
	struct mixer_state *state = &machine->mixer.state;

	/* The Microwire begins shifting on the first cycle. */
	return mixer_cycle.c < state->microwire.cycle.c + 15 ?
		1 + mixer_cycle.c - state->microwire.cycle.c: 0;
}

static bool microwire_busy(struct machine *machine,
	const struct device_cycle mixer_cycle)
{
	return microwire_shift(machine, mixer_cycle) != 0;
}

static void mixer_emit(struct machine *machine,
	const struct device_cycle mixer_cycle)
{
	struct mixer_state *state = &machine->mixer.state;
	struct mixer_sample buffer[256];
	size_t count = 0;

	for (u64 c = ALIGN(machine->mixer.mixer_emit_latest_cycle,
			   MIXER_SAMPLE_CYCLES);
	     c < mixer_cycle.c;
	     c += MIXER_SAMPLE_CYCLES) {
		const struct device_cycle sample_cycle =
			(struct device_cycle) { .c = c };

		if (!microwire_busy(machine, sample_cycle))
			state->sample = state->microwire.sample;

		buffer[count++] = state->sample;

		if (count >= ARRAY_SIZE(buffer)) {
			if (machine->mixer.output.sample)
				machine->mixer.output.sample(buffer, count,
					machine->mixer.output.sample_arg);
			count = 0;
		}
	}

	if (count && machine->mixer.output.sample)
		machine->mixer.output.sample(buffer,
			count, machine->mixer.output.sample_arg);

	const u32 shift = microwire_shift(machine, mixer_cycle);
	if (shift) {
		/* Shift data and mask words during 16 us transmission. */
		state->microwire.rd.data =
			(state->microwire.wr.data << shift) |
			(state->microwire.wr.data >> (16 - shift));
		state->microwire.rd.mask =
			(state->microwire.wr.mask << shift) |
			(state->microwire.wr.mask >> (16 - shift));
	} else
		state->microwire.rd = state->microwire.wr;

	machine->mixer.mixer_emit_latest_cycle = mixer_cycle.c;
}

static void mixer_event(struct machine *machine, const struct device *device,
	const struct device_cycle mixer_cycle)
{
	mixer_emit(machine, mixer_cycle);

	request_device_event(machine, device, (struct device_cycle) {
			.c = mixer_cycle.c + MIXER_EVENT_CYCLES
		});
}

static uint16_t microwire_cmd(struct machine *machine, const struct device *device)
{
	struct mixer_state *state = &machine->mixer.state;
	uint16_t cmd = 0;

	for (int i = 0; i < 16; i++)
		if (state->microwire.wr.mask & (0x8000 >> i)) {
			cmd <<= 1;
			cmd |= (state->microwire.wr.data & (0x8000 >> i) ? 1 : 0);
		}

	return cmd;
}

static void microwire(struct machine *machine, const struct device *device,
	const struct device_cycle mixer_cycle)
{
	struct mixer_state *state = &machine->mixer.state;

	state->microwire.cycle = (struct device_cycle) { .c = mixer_cycle.c };

	const uint16_t cmd = microwire_cmd(machine, device);
	const uint16_t addr = cmd >> 9;
	const uint16_t reg = (cmd >> 6) & 0x7;
	const s16 data = cmd & 0x3f;

	if (addr != 2)
		return;

	switch (reg)  {
	case MIXER_LMC1992_REG_MIXER:
		state->microwire.sample.mix = ((data & 0x3) == 1);
		break;

	case MIXER_LMC1992_REG_BASS:
		state->microwire.sample.tone.bass =
			clamp(2 * (data & 0xf) - 12, -12, 12);
		break;

	case MIXER_LMC1992_REG_TREBLE:
		state->microwire.sample.tone.treble =
			clamp(2 * (data & 0xf) - 12, -12, 12);
		break;

	case MIXER_LMC1992_REG_VOLUME_MAIN:
		state->microwire.sample.volume.main =
			clamp(2 * (data & 0x3f) - 80, -80, 0);
		break;

	case MIXER_LMC1992_REG_VOLUME_RIGHT:
		state->microwire.sample.volume.right =
			clamp(2 * (data & 0x1f) - 40, -40, 0);
		break;

	case MIXER_LMC1992_REG_VOLUME_LEFT:
		state->microwire.sample.volume.left =
			clamp(2 * (data & 0x1f) - 40, -40, 0);
		break;
	}
}

static uint8_t mixer_rd_u8(struct machine *machine, const struct device *device,
	u32 dev_address)
{
	struct mixer_state *state = &machine->mixer.state;

	mixer_emit(machine, device_cycle(machine, device));

	return dev_address < ARRAY_SIZE(state->microwire.rd.byte) ?
		state->microwire.rd.byte[dev_address] : 0;
}

static uint16_t mixer_rd_u16(struct machine *machine, const struct device *device,
	u32 dev_address)
{
	struct mixer_state *state = &machine->mixer.state;

	const u32 reg = dev_address >> 1;

	mixer_emit(machine, device_cycle(machine, device));

	return reg < ARRAY_SIZE(state->microwire.rd.halfword) ?
		state->microwire.rd.halfword[reg] : 0;
}

static void mixer_wr_u8(struct machine *machine, const struct device *device,
	u32 dev_address, uint8_t data)
{
	struct mixer_state *state = &machine->mixer.state;
	const struct device_cycle mixer_cycle = device_cycle(machine, device);
	const u32 reg = dev_address >> 1;

	/* Writing is ignored during the 16 us Microwire transmission. */
	if (microwire_busy(machine, mixer_cycle))
		return;

	if (ARRAY_SIZE(state->microwire.wr.byte) <= dev_address)
		return;

	state->microwire.wr.byte[dev_address] = data;

	if (reg == MIXER_MICROWIRE_REG_DATA)
		microwire(machine, device, mixer_cycle);
}

static void mixer_wr_u16(struct machine *machine, const struct device *device,
	u32 dev_address, uint16_t data)
{
	struct mixer_state *state = &machine->mixer.state;
	const struct device_cycle mixer_cycle = device_cycle(machine, device);
	const u32 reg = dev_address >> 1;

	/* Writing is ignored during the 16 us Microwire transmission. */
	if (microwire_busy(machine, mixer_cycle))
		return;

	if (ARRAY_SIZE(state->microwire.wr.halfword) <= reg)
		return;

	state->microwire.wr.halfword[reg] = data;

	if (reg == MIXER_MICROWIRE_REG_DATA)
		microwire(machine, device, mixer_cycle);
}

static size_t mixer_id_u8(struct machine *machine, const struct device *device,
	u32 dev_address, char *buf, size_t size)
{
	const u32 reg = dev_address >> 1;

	if (reg == MIXER_MICROWIRE_REG_DATA ||
	    reg == MIXER_MICROWIRE_REG_MASK)
		snprintf(buf, size, "%s", mixer_register_name(reg));
	else
		snprintf(buf, size, "%2u", dev_address);

	return strlen(buf);
}

static size_t mixer_id_u16(struct machine *machine, const struct device *device,
	u32 dev_address, char *buf, size_t size)
{
	const u32 reg = dev_address >> 1;

	if (reg == MIXER_MICROWIRE_REG_DATA ||
	    reg == MIXER_MICROWIRE_REG_MASK)
		snprintf(buf, size, "%s", mixer_register_name(reg));
	else
		snprintf(buf, size, "%2u", dev_address);

	return strlen(buf);
}

static void mixer_reset(struct machine *machine, const struct device *device)
{
	struct mixer_state *state = &machine->mixer.state;

	*state = (struct mixer_state) { };

	state->microwire.sample.mix = true;
	state->sample = state->microwire.sample;

	machine->mixer.mixer_emit_latest_cycle = 0;

	mixer_event(machine, device, device_cycle(machine, device));
}

void mixer_sample(struct machine *machine,
	mixer_sample_f sample, void *sample_arg)
{
	machine->mixer.output.sample = sample;
	machine->mixer.output.sample_arg = sample_arg;
}

const struct device mixer_device = {
	.name = "mixer",
	.clk = {
		.frequency = MIXER_FREQUENCY,
		.divisor = 1
	},
	.bus = {
		.address = 0xff8922,
		.size = 4,
	},
	.reset = mixer_reset,
	.event = mixer_event,
	.rd_u8  = mixer_rd_u8,
	.rd_u16 = mixer_rd_u16,
	.wr_u8  = mixer_wr_u8,
	.wr_u16 = mixer_wr_u16,
	.id_u8  = mixer_id_u8,
	.id_u16 = mixer_id_u16,
};
