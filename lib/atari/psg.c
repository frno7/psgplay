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

#include "cf2149/module/cf2149.h"

#define PSG_EVENT_FREQUENCY 100		/* 10 ms */
#define PSG_EVENT_CYCLES (PSG_FREQUENCY / PSG_EVENT_FREQUENCY)

static struct cf2149_module cf2149;

static struct {
	psg_sample_f sample;
	void *sample_arg;
} output;

static char *psg_register_name(u32 reg)
{
	switch (reg) {
#define PSG_REG_NAME(register_, symbol_, label_, description_)		\
	case register_: return #symbol_;
CF2149_REGISTERS(PSG_REG_NAME)
	default:
		return "";
	}
}

static void psg_emit(const struct device_cycle psg_cycle)
{
	const struct cf2149_clk clk = cf2149_clk_cycle(psg_cycle.c);
	struct cf2149_ac buffer[256];

	for (;;) {
		const size_t n = cf2149.port.rd_ac(&cf2149,
			clk, &buffer[0], ARRAY_SIZE(buffer));

		if (!n)
			return;

		if (output.sample)
			output.sample(buffer, n, output.sample_arg);
	}
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
	const struct device_cycle psg_cycle = device_cycle(device);
	const struct cf2149_clk clk = cf2149_clk_cycle(psg_cycle.c);

	switch (dev_address) {
	case 0:
	case 1:
		cf2149.port.bdc(&cf2149, clk,
			(union cf2149_bdc) { .u8 = CF2149_BDC_DTB });
		return cf2149.port.rd_da(&cf2149, clk);
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
	const struct cf2149_clk clk = cf2149_clk_cycle(psg_cycle.c);

	psg_emit(psg_cycle);

	switch (dev_address % 4) {
	case 0:
	case 1:
		cf2149.port.bdc(&cf2149, clk,
			(union cf2149_bdc) { .u8 = CF2149_BDC_BAR });
		cf2149.port.wr_da(&cf2149, clk, data);
		break;
	case 2:
	case 3:
		cf2149.port.bdc(&cf2149, clk,
			(union cf2149_bdc) { .u8 = CF2149_BDC_DWS });
		cf2149.port.wr_da(&cf2149, clk, data);
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
		snprintf(buf, size, "wr %s",
			psg_register_name(cf2149.state.reg_address));
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
	const struct device_cycle psg_cycle = device_cycle(device);
	const struct cf2149_clk clk = cf2149_clk_cycle(psg_cycle.c);

	cf2149 = cf2149_init();

	cf2149.port.select_l(&cf2149, clk, CF2149_SELECT_MODE_CLKDIV8);

	psg_event(device, device_cycle(device));
}

void psg_sample(psg_sample_f sample, void *sample_arg)
{
	output.sample = sample;
	output.sample_arg = sample_arg;
}

const struct device psg_device = {
	.name = "psg",
	.clk = {
		.frequency = PSG_FREQUENCY,
		.divisor = 1
	},
	.bus = {
		.address = 0xff8800,
		.size = 256,
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
