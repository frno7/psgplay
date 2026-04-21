// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#ifndef ATARI_BUS_H
#define ATARI_BUS_H

#include "internal/types.h"

#include "atari/machine.h"
#include "atari/device.h"

uint8_t bus_error_rd_u8(struct machine *machine, const struct device *device,
	uint32_t address);
uint16_t bus_error_rd_u16(struct machine *machine, const struct device *device,
	uint32_t address);

void bus_error_wr_u8(struct machine *machine, const struct device *device,
	uint32_t address, uint8_t data);
void bus_error_wr_u16(struct machine *machine, const struct device *device,
	uint32_t address, uint16_t data);

extern const struct device bus_device_error;

#endif /* ATARI_BUS_H */
