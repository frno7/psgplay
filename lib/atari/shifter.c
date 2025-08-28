// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#include <stdio.h>
#include <string.h>

#include "toslibc/asm/machine.h"

#include "atari/device.h"
#include "atari/shifter.h"

static char *shifter_register_name(u32 reg)
{
	switch (reg) {
#define SHIFTER_REG_NAME(register_, symbol_, label_, description_)	\
	case register_: return #symbol_;
SHIFTER_REGISTERS(SHIFTER_REG_NAME)
	default:
		return "";
	}
}

static u8 shifter_rd_u8(const struct device *device, u32 dev_address)
{
	return 0;	/* FIXME */
}

static u16 shifter_rd_u16(const struct device *device, u32 dev_address)
{
	return 0;	/* FIXME */
}

static void shifter_wr_u8(const struct device *device, u32 dev_address, u8 data)
{
	/* FIXME */
}

static void shifter_wr_u16(const struct device *device, u32 dev_address, u16 data)
{
	/* FIXME */
}

static size_t shifter_id_u16(const struct device *device,
	u32 dev_address, char *buf, size_t size)
{
	snprintf(buf, size, "wr %s", shifter_register_name(dev_address / 2));	/* FIXME */

	return strlen(buf);
}

static size_t shifter_id_u8(const struct device *device,
	u32 dev_address, char *buf, size_t size)
{
	snprintf(buf, size, "wr %s", shifter_register_name(dev_address / 2));	/* FIXME */

	return strlen(buf);
}

static void shifter_event(const struct device *device,
	const struct device_cycle mfp_cycle)
{
}

static void shifter_reset(const struct device *device)
{
}

const struct device shifter_device = {
	.name = "shifter",
	.clk = {
		.frequency = ATARI_STE_PAL_MCLK,
		.divisor = 1
	},
	.bus = {
		.address = 0xff8200,
		.size = 512,
	},
	.reset = shifter_reset,
	.event = shifter_event,
	.rd_u8  = shifter_rd_u8,
	.rd_u16 = shifter_rd_u16,
	.wr_u8  = shifter_wr_u8,
	.wr_u16 = shifter_wr_u16,
	.id_u8  = shifter_id_u8,
	.id_u16 = shifter_id_u16,
};
