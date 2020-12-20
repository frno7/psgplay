// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#include <stdio.h>
#include <string.h>

#include "internal/build-assert.h"

#include "atari/bus.h"
#include "atari/device.h"
#include "atari/fdc.h"

static void fdc_event(const struct device *device,
	const struct device_cycle mfp_cycle)
{
}

static u8 fdc_rd_u8(const struct device *device, u32 dev_address)
{
	return 0;
}

static u16 fdc_rd_u16(const struct device *device, u32 dev_address)
{
	return fdc_rd_u8(device, dev_address) << 8;
}

static void fdc_wr_u8(const struct device *device, u32 dev_address, u8 data)
{
}

static void fdc_wr_u16(const struct device *device, u32 dev_address, u16 data)
{
	fdc_wr_u8(device, dev_address, data >> 8);
}

static size_t fdc_id_u8(const struct device *device,
	u32 dev_address, char *buf, size_t size)
{
	buf[0] = '\0';

	return strlen(buf);
}

static size_t fdc_id_u16(const struct device *device,
	u32 dev_address, char *buf, size_t size)
{
	return fdc_id_u8(device, dev_address, buf, size);
}

static void fdc_reset(const struct device *device)
{
}

const struct device fdc_device = {
	.name = "fdc",
	.frequency = 8000000,
	.bus = {
		.address = 0xff8600,
		.size = 16,
	},
	.reset = fdc_reset,
	.event = fdc_event,
	.rd_u8  = fdc_rd_u8,
	.rd_u16 = fdc_rd_u16,
	.wr_u8  = fdc_wr_u8,
	.wr_u16 = fdc_wr_u16,
	.id_u8  = fdc_id_u8,
	.id_u16 = fdc_id_u16,
};
