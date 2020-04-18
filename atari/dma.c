// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#include <stdio.h>
#include <string.h>

#include "atari/bus.h"
#include "atari/device.h"
#include "atari/dma.h"

#include "psgplay/build-bug.h"

static void dma_event(const struct device *device,
	const struct device_cycle mfp_cycle)
{
}

static u8 dma_rd_u8(const struct device *device, u32 dev_address)
{
	return 0;
}

static u16 dma_rd_u16(const struct device *device, u32 dev_address)
{
	return dma_rd_u8(device, dev_address) << 8;
}

static void dma_wr_u8(const struct device *device, u32 dev_address, u8 data)
{
}

static void dma_wr_u16(const struct device *device, u32 dev_address, u16 data)
{
	dma_wr_u8(device, dev_address, data >> 8);
}

static size_t dma_id_u8(const struct device *device,
	u32 dev_address, char *buf, size_t size)
{
	buf[0] = '\0';

	return strlen(buf);
}

static size_t dma_id_u16(const struct device *device,
	u32 dev_address, char *buf, size_t size)
{
	return dma_id_u8(device, dev_address, buf, size);
}

static void dma_reset(const struct device *device)
{
}

const struct device dma_device = {
	.name = "dma",
	.frequency = 8000000,
	.bus = {
		.address = 0xff8600,
		.size = 16,
	},
	.reset = dma_reset,
	.event = dma_event,
	.rd_u8  = dma_rd_u8,
	.rd_u16 = dma_rd_u16,
	.wr_u8  = dma_wr_u8,
	.wr_u16 = dma_wr_u16,
	.id_u8  = dma_id_u8,
	.id_u16 = dma_id_u16,
};
