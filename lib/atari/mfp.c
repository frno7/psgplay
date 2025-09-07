// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#include <stdio.h>
#include <string.h>

#include "internal/assert.h"
#include "internal/build-assert.h"
#include "internal/types.h"

#include "atari/bus.h"
#include "atari/device.h"
#include "atari/irq.h"
#include "atari/glue.h"
#include "atari/machine.h"
#include "atari/mfp.h"
#include "atari/mfp-map.h"

#include "cf68901/module/cf68901.h"

#define MFP_FREQUENCY	(CPU_FREQUENCY / 2)

#define GPIP_MONITOR_DETECT 7

static struct cf68901_module cf68901;

static bool mono_monitor_detect()
{
	return true;	/* FIXME: Only color monitor as of now */
}

static char *mfp_register_name(u32 reg)
{
	switch (reg) {
#define MFP_REG_NAME(register_, symbol_, label_, description_)		\
	case register_: return #symbol_;
CF68901_REGISTERS(MFP_REG_NAME)
	default:
		return "";
	}
}

uint32_t mfp_irq_vector(void)
{
	return cf68901.port.vector(&cf68901);
}

static void request_event(const struct device *device,
	struct cf68901_event event)
{
	if (event.clk.c)
		request_device_event(device,
			(struct device_cycle) { .c = event.clk.c });
	(event.irq ? glue_irq_set : glue_irq_clr)(IRQ_MFP);
}

static void mfp_event(const struct device *device,
	const struct device_cycle mfp_cycle)
{
	const struct cf68901_clk clk = cf68901_clk_cycle(mfp_cycle.c);

	request_event(device, cf68901.port.event(&cf68901, clk));
}

static u8 mfp_rd_u8(const struct device *device, u32 dev_address)
{
	const struct device_cycle mfp_cycle = device_cycle(device);
	const struct cf68901_clk clk = cf68901_clk_cycle(mfp_cycle.c);
	const u32 reg = dev_address >> 1;

	if ((dev_address & 1) == 0)
		return 0;

	return cf68901.port.rd_da(&cf68901, clk, reg);
}

static u16 mfp_rd_u16(const struct device *device, u32 dev_address)
{
	return mfp_rd_u8(device, dev_address + 1);
}

static void mfp_wr_u8(const struct device *device,
	u32 dev_address, u8 data)
{
	const struct device_cycle mfp_cycle = device_cycle(device);
	const struct cf68901_clk clk = cf68901_clk_cycle(mfp_cycle.c);
	const u32 reg = dev_address >> 1;

	if ((dev_address & 1) == 0)
		return;

	request_event(device, cf68901.port.wr_da(&cf68901, clk, reg, data));
}

static void mfp_wr_u16(const struct device *device, u32 dev_address, u16 data)
{
	mfp_wr_u8(device, dev_address + 1, data & 0xff);
}

static size_t mfp_id_u8(const struct device *device,
	u32 dev_address, char *buf, size_t size)
{
	const u32 reg = dev_address >> 1;

	if ((dev_address & 1) == 0 || 24 <= reg)
		snprintf(buf, size, "%2u", dev_address);
	else
		snprintf(buf, size, "%s", mfp_register_name(reg));

	return strlen(buf);
}

static size_t mfp_id_u16(const struct device *device,
	u32 dev_address, char *buf, size_t size)
{
	return mfp_id_u8(device, dev_address + 1, buf, size);
}

static void mfp_reset(const struct device *device)
{
	const struct device_cycle mfp_cycle = device_cycle(&mfp_device);
	const struct cf68901_clk clk = cf68901_clk_cycle(mfp_cycle.c);

	cf68901 = cf68901_init(MFP_FREQUENCY, MFP_TIMER_FREQUENCY);

	cf68901.port.wr_gpip(&cf68901, clk,
		GPIP_MONITOR_DETECT, mono_monitor_detect());
}

const struct device mfp_device = {
	.name = "mfp",
	.clk = {
		.frequency = MFP_FREQUENCY,
		.divisor = 1
	},
	.bus = {
		.address = MFP_BUS_ADDRESS,
		.size = 64,
	},
	.reset = mfp_reset,
	.event = mfp_event,
	.rd_u8  = mfp_rd_u8,
	.rd_u16 = mfp_rd_u16,
	.wr_u8  = mfp_wr_u8,
	.wr_u16 = mfp_wr_u16,
	.id_u8  = mfp_id_u8,
	.id_u16 = mfp_id_u16,
};

void dma_sound_active(bool level)
{
	static bool prev_level = 0;

	if (level == prev_level)
		return;

	const struct device_cycle mfp_cycle = device_cycle(&mfp_device);
	const struct cf68901_clk clk = cf68901_clk_cycle(mfp_cycle.c);

	request_event(&mfp_device, cf68901.port.tai(&cf68901, clk, level));
	request_event(&mfp_device, cf68901.port.wr_gpip(&cf68901, clk,
		GPIP_MONITOR_DETECT, level ^ mono_monitor_detect()));

	prev_level = level;
}
