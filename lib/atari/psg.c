// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "internal/macro.h"

#include "atari/bus.h"
#include "atari/device.h"
#include "atari/irq.h"
#include "atari/glue.h"
#include "atari/machine.h"
#include "atari/psg.h"

#include "psgplay/assert.h"

#define PSG_FREQUENCY	2000000

struct psg_state {
	union psg psg;

	u8 reg_select;

	struct device_cycle env_cycle;
};

static union psg psg;
static u8 reg_select;
static struct device_cycle env_cycle;

static struct {
	int sample_frequency;
	sample_f sample;
	void *sample_arg;
	bool halt;
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

	const u64 ei = (psg_cycle.c - env_cycle.c) >> 4;
	const u64 si = ei / psg_env_period();
	const u32 wi = si < 16 ? si : 16 + ((16 + si) % 32);

	return wave[psg.envelope_shape.ctrl][wi];
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

static s16 lowpass(const s16 x0)
{
	static s16 xn[8] = { };
	static int k = 0;

	xn[k++ % ARRAY_SIZE(xn)] = x0;

	s32 x = 0;
	for (int i = 0; i < ARRAY_SIZE(xn); i++)
		x += xn[i];		/* Simplistic 8 tap FIR filter. */

	return x / ARRAY_SIZE(xn);
}

static s16 psg_dac(const u8 level)
{
	/*
	 * Table is computed with 2^[ (lvl-15)/2 ] although the levels in
	 * figure 9 in AY-3-8910/8912 Programmable Sound Generator Data Manual,
	 * February 1979, p. 29 are slightly different as shown in the comment.
	 */

	static const u16 dac[16] = {
		0.006 * 0xffff,		/* 0.000 */
		0.008 * 0xffff,		/* ..... */
		0.011 * 0xffff,		/* ..... */
		0.016 * 0xffff,		/* ..... */
		0.022 * 0xffff,		/* ..... */
		0.031 * 0xffff,		/* ..... */
		0.044 * 0xffff,		/* ..... */
		0.062 * 0xffff,		/* ..... */
		0.088 * 0xffff,		/* ..... */
		0.125 * 0xffff,		/* 0.125 */
		0.177 * 0xffff,		/* 0.152 */
		0.250 * 0xffff,		/* 0.250 */
		0.354 * 0xffff,		/* 0.303 */
		0.500 * 0xffff,		/* 0.500 */
		0.707 * 0xffff,		/* 0.707 */
		1.000 * 0xffff,		/* 1.000 */
	};

	return (level < 16 ? dac[level] : 0xffff) - 0x8000;
}

static s16 psg_dac3(const u8 lva, const u8 lvb, const u8 lvc)
{
	const s16 sa = psg_dac(lva);
	const s16 sb = psg_dac(lvb);
	const s16 sc = psg_dac(lvc);

	return (sa + sb + sc) / 3;	/* Simplistic linear channel mix. */
}

static s16 fade_in(const s16 sample) /* FIXME: Do in replay, with fade_out */
{
	static s16 v = 0;

	return v < 0x4000 ? (sample * v++) / 0x4000 : sample;
}

static void downsample(const struct device_cycle psg_cycle, const s16 sample)
{
	static u64 sample_cycle = 0;

	const u64 n = (output.sample_frequency * psg_cycle.c) / PSG_FREQUENCY;

	for (; sample_cycle < n && !output.halt; sample_cycle++)
		if (!output.sample(sample, sample, output.sample_arg))
			output.halt = true;
}

static void psg_emit_cycle(const struct device_cycle psg_cycle)
{
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

	downsample(psg_cycle, fade_in(lowpass(psg_dac3(lva, lvb, lvc))));
}

static void psg_emit(const struct device_cycle psg_cycle)
{
	static u64 last_cycle = 0;

	for (u64 c = ALIGN(last_cycle, 8); c < psg_cycle.c; c += 8)
		psg_emit_cycle((struct device_cycle) { .c = c });

	last_cycle = psg_cycle.c;
}

static void psg_event(const struct device *device,
	const struct device_cycle psg_cycle)
{
	psg_emit(psg_cycle);

	request_device_event(device,
		(struct device_cycle) { .c = psg_cycle.c + 10000 });
}

static u8 psg_rd_u8(const struct device *device, u32 dev_address)
{
	switch (dev_address) {
	case 0: return reg_select < 16 ? psg.reg[reg_select] : 0xff;
	case 2: return 0xff;
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
		reg_select = data;
		break;
	case 2:
		if (reg_select == 13)
			env_cycle = psg_cycle;
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
		snprintf(buf, size, "rd/select");
		break;
	case 2:
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

	psg_event(device, device_cycle(device));
}

void psg_sample(int sample_frequency, sample_f sample, void *sample_arg)
{
	output.sample_frequency = sample_frequency;
	output.sample = sample;
	output.sample_arg = sample_arg;

	output.halt = false;
}

bool psg_output_halt(void)
{
	return output.halt;
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
