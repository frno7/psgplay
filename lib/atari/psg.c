// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
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
#include "atari/psg.h"

#define PSG_EVENT_FREQUENCY 100		/* 10 ms */
#define PSG_EVENT_CYCLES (PSG_FREQUENCY / PSG_EVENT_FREQUENCY)

struct psg_state {
	union psg psg;

	u8 reg_select;
};

static union psg psg;
static u8 reg_select;
static u32 env_index;
static u32 env_wave_index;

static struct {
	psg_sample_f sample;
	void *sample_arg;
} output;

static char *psg_register_name(u32 reg)
{
	switch (reg) {
#define PSG_REG_NAME(register_, symbol_, label_, description_)		\
	case register_: return #symbol_;
PSG_REGISTERS(PSG_REG_NAME)
	default:
		return "";
	}
}

#define DEFINE_PSG_CHN_PERIOD(channel_)					\
static u16 psg_ch ## channel_ ## _period(void)				\
{									\
	const u16 period = (psg.hi_ ## channel_.period << 8) |		\
			    psg.lo_ ## channel_.period;			\
									\
	return period ? period : 1;					\
}

DEFINE_PSG_CHN_PERIOD(a)
DEFINE_PSG_CHN_PERIOD(b)
DEFINE_PSG_CHN_PERIOD(c)

#define DEFINE_PSG_CHN_UPDATE(channel_)					\
static bool psg_ch ## channel_ ## _update(				\
	const struct device_cycle psg_cycle)				\
{									\
	static u16 cycle = 0;						\
									\
	const u16 period = psg_ch ## channel_ ## _period();		\
	const bool t = cycle < period;					\
									\
	if (++cycle >= 2 * period)					\
		cycle = 0;						\
									\
	return t;							\
}

DEFINE_PSG_CHN_UPDATE(a)
DEFINE_PSG_CHN_UPDATE(b)
DEFINE_PSG_CHN_UPDATE(c)

#define DEFINE_PSG_MXN(channel_)					\
static bool psg_mx ## channel_(const bool t, const bool n)		\
{									\
	return (psg.iomix.tone_ ## channel_ || t) &&			\
	       (psg.iomix.noise_ ## channel_ || n);			\
}

DEFINE_PSG_MXN(a)
DEFINE_PSG_MXN(b)
DEFINE_PSG_MXN(c)

static u16 psg_noise_period(void)
{
	return psg.noise.period ? psg.noise.period : 1;
}

/* 17 stage 2 tap LFSR with bits 17 and 14, having period 131072. */
static bool psg_rng_update(const struct device_cycle psg_cycle)
{
	static u32 rng = 1;
	static u32 cycle = 0;

	const bool r = rng & 1;

	if (++cycle >= 2 * psg_noise_period()) {
		if (r)
			rng = (rng >> 1) ^ 0x12000;
		else
			rng >>= 1;

		cycle = 0;
	}

	return r;
}

static u16 psg_env_period(void)
{
	const u16 period = (psg.envelope_hi.period << 8) |
			    psg.envelope_lo.period;

	return period ? period : 1;
}

static u8 psg_env_level(const struct device_cycle psg_cycle)
{
#define RISE  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15
#define FALL 15, 14, 13, 12, 11, 10,  9 , 8 , 7 , 6,  5,  4,  3,  2,  1,  0
#define ZERO  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
#define HOLD 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15

	static const u8 wave[16][3 * 16] = {
		{ FALL, ZERO, ZERO },	/* \___ */
		{ FALL, ZERO, ZERO },	/* \___ */
		{ FALL, ZERO, ZERO },	/* \___ */
		{ FALL, ZERO, ZERO },	/* \___ */
		{ RISE, ZERO, ZERO },	/* /___ */
		{ RISE, ZERO, ZERO },	/* /___ */
		{ RISE, ZERO, ZERO },	/* /___ */
		{ RISE, ZERO, ZERO },	/* /___ */
		{ FALL, FALL, FALL },	/* \\\\ */
		{ FALL, ZERO, ZERO },	/* \___ */
		{ FALL, RISE, FALL },	/* \/\/ */
		{ FALL, HOLD, HOLD },	/* \--- */
		{ RISE, RISE, RISE },	/* //// */
		{ RISE, HOLD, HOLD },	/* /--- */
		{ RISE, FALL, RISE },	/* /\/\ */
		{ RISE, ZERO, ZERO },	/* /___ */
	};

	if (env_index >= psg_env_period()) {
		env_index = 0;

		if (++env_wave_index >= 3 * 16)
			env_wave_index -= 2 * 16;
	}

	if ((psg_cycle.c & 0xf) == 0x8)
		env_index++;

	return wave[psg.envelope_shape.ctrl][env_wave_index];
}

#define PSG_LVN(channel_)						\
static u8 psg_lv ## channel_(const bool mx, const u8 env)		\
{									\
	return mx ? (psg.level_ ## channel_.m ?				\
			env : psg.level_ ## channel_.level) : 0;	\
}

PSG_LVN(a)
PSG_LVN(b)
PSG_LVN(c)

static u64 psg_emit_latest_cycle;
static void psg_emit(const struct device_cycle psg_cycle)
{
	struct psg_sample buffer[256];
	size_t count = 0;

	for (u64 c = ALIGN(psg_emit_latest_cycle, 8); c < psg_cycle.c; c += 8) {
		const struct device_cycle psg_cycle = { .c = c };

		const bool cha = psg_cha_update(psg_cycle);
		const bool chb = psg_chb_update(psg_cycle);
		const bool chc = psg_chc_update(psg_cycle);
		const bool rng = psg_rng_update(psg_cycle);

		const bool mxa = psg_mxa(cha, rng);
		const bool mxb = psg_mxb(chb, rng);
		const bool mxc = psg_mxc(chc, rng);

		const u8 env = psg_env_level(psg_cycle);
		const u8 lva = psg_lva(mxa, env);
		const u8 lvb = psg_lvb(mxb, env);
		const u8 lvc = psg_lvc(mxc, env);

		buffer[count++] = (struct psg_sample) {
			.lva = lva,
			.lvb = lvb,
			.lvc = lvc,
		};

		if (count >= ARRAY_SIZE(buffer)) {
			if (output.sample)
				output.sample(buffer, count, output.sample_arg);
			count = 0;
		}
	}

	if (count && output.sample)
		output.sample(buffer, count, output.sample_arg);

	psg_emit_latest_cycle = psg_cycle.c;
}

static void psg_event(const struct device *device,
	const struct device_cycle psg_cycle)
{
	psg_emit(psg_cycle);

	request_device_event(device,
		(struct device_cycle) { .c = psg_cycle.c + PSG_EVENT_CYCLES });
}

static u8 psg_rd_u8(const struct device *device, u32 dev_address)
{
	switch (dev_address) {
	case 0:
	case 1: return reg_select < 16 ? psg.reg[reg_select] : 0xff;
	case 2:
	case 3: return 0xff;
	default:
		BUG();
	}
}

static u16 psg_rd_u16(const struct device *device, u32 dev_address)
{
	return psg_rd_u8(device, dev_address) << 8;
}

static void psg_wr_u8(const struct device *device, u32 dev_address, u8 data)
{
	const struct device_cycle psg_cycle = device_cycle(device);

	psg_emit(psg_cycle);

	switch (dev_address % 4) {
	case 0:
	case 1:
		reg_select = data;
		break;
	case 2:
	case 3:
		if (reg_select == PSG_REG_SHAPE && psg.reg[reg_select] != data)
			env_index = env_wave_index = 0;

		if (reg_select < 16)
			psg.reg[reg_select] = data;
		break;
	default:
		BUG();
	}
}

static void psg_wr_u16(const struct device *device, u32 dev_address, u16 data)
{
	psg_wr_u8(device, dev_address, data >> 8);
}

static size_t psg_id_u8(const struct device *device,
	u32 dev_address, char *buf, size_t size)
{
	switch (dev_address % 4) {
	case 0:
	case 1:
		snprintf(buf, size, "rd/select");
		break;
	case 2:
	case 3:
		snprintf(buf, size, "wr %s", psg_register_name(reg_select));
		break;
	default:
		snprintf(buf, size, "%2u", dev_address);
		BUG();
	}

	return strlen(buf);
}

static size_t psg_id_u16(const struct device *device,
	u32 dev_address, char *buf, size_t size)
{
	return psg_id_u8(device, dev_address, buf, size);
}

static void psg_reset(const struct device *device)
{
	BUILD_BUG_ON(sizeof(psg) != 16);

	memset(&psg, 0, sizeof(psg));

	psg.reg[PSG_REG_IOMIX] = 0xff;
	env_index = env_wave_index = 0;

	psg_emit_latest_cycle = 0;

	psg_event(device, device_cycle(device));
}

void psg_sample(psg_sample_f sample, void *sample_arg)
{
	output.sample = sample;
	output.sample_arg = sample_arg;
}

const struct device psg_device = {
	.name = "psg",
	.frequency = PSG_FREQUENCY,
	.bus = {
		.address = 0xff8800,
		.size = 256,
	},
	.state = {
		.size = sizeof(struct psg_state),
	},
	.reset = psg_reset,
	.event = psg_event,
	.rd_u8  = psg_rd_u8,
	.rd_u16 = psg_rd_u16,
	.wr_u8  = psg_wr_u8,
	.wr_u16 = psg_wr_u16,
	.id_u8  = psg_id_u8,
	.id_u16 = psg_id_u16,
};
