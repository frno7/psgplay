// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#ifndef ATARI_DEVICE_H
#define ATARI_DEVICE_H

#include "internal/types.h"

#include "atari/machine.h"

struct device_cycle {
	u64 c;
};

struct device_slice {
	u64 s;
};

struct device_state {
	void *internal;
	size_t size;
};

struct device {
	const char *name;

	bool main_bus;
	struct {
		u32 frequency;
		u32 divisor;
	} clk;

	struct {
		u32 address;
		u32 size;
	} bus;

	struct device_state state;

	struct device_slice (*run)(struct machine *machine,
		const struct device *device, struct device_cycle,
		struct device_slice);
	void (*reset)(struct machine *machine, const struct device *device);
	void (*event)(struct machine *machine, const struct device *device,
		struct device_cycle device_cycle);

	u8 (*rd_u8)(struct machine *machine, const struct device *device,
		u32 dev_address);
	u16 (*rd_u16)(struct machine *machine,const struct device *device,
		u32 dev_address);

	void (*wr_u8)(struct machine *machine, const struct device *device,
		u32 dev_address, u8 data);
	void (*wr_u16)(struct machine *machine, const struct device *device,
		u32 dev_address, u16 data);

	size_t (*id_u8)(struct machine *machine, const struct device *device,
		u32 dev_address, char *buf, size_t size);
	size_t (*id_u16)(struct machine *machine, const struct device *device,
		u32 dev_address, char *buf, size_t size);
};

bool valid_device_bus_address(u32 bus_address, const struct device *dev);

const struct device *device_for_bus_address(u32 bus_address);

struct device_cycle device_cycle(const struct device *device);

struct device_cycle device_from_machine_cycle(
	const struct device *device, u64 machine_cycle);

void request_device_event(const struct device *device,
	struct device_cycle device_cycle);

void device_reset(struct machine *machine);

u64 device_run(struct machine *machine, u64 machine_cycle, u64 machine_slice);

#endif /* ATARI_DEVICE_H */
