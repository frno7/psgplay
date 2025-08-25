// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#ifndef ATARI_BUS_H
#define ATARI_BUS_H

#include "internal/types.h"

#include "atari/machine.h"
#include "atari/device.h"

#define BUS_FREQUENCY CPU_FREQUENCY

u8 bus_error_rd_u8(const struct device *device, u32 address);
u16 bus_error_rd_u16(const struct device *device, u32 address);
void bus_error_wr_u8(const struct device *device, u32 address, u8 data);
void bus_error_wr_u16(const struct device *device, u32 address, u16 data);

extern const struct device bus_device_error;

#endif /* ATARI_BUS_H */
