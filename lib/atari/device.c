// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#include "internal/assert.h"
#include "internal/compare.h"
#include "internal/struct.h"
#include "internal/types.h"

#include "atari/bus.h"
#include "atari/cpu.h"
#include "atari/fdc.h"
#include "atari/glue.h"
#include "atari/machine.h"
#include "atari/mfp.h"
#include "atari/psg.h"
#include "atari/ram.h"
#include "atari/rom.h"
#include "atari/shifter.h"

static struct machine_device {
	u64 machine_cycle_event;
	const struct device *device;
} list[] = {
	{ .device = &rom0_device    },
	{ .device = &rom1_device    },
	{ .device = &glue_device    },
	{ .device = &ram_device     },
	{ .device = &mfp_device     },
	{ .device = &shifter_device },
	{ .device = &psg_device     },
	{ .device = &fdc_device     },
	{ .device = &cpu_device     },
};

#define for_each_device(device_)					\
	for (struct machine_device *machine_device_ = &list[0];		\
	     (machine_device_) - &list[0] < ARRAY_SIZE(list) &&		\
	     ((device_) = machine_device_->device);			\
	     (machine_device_)++)

#define for_each_device_state(machine_device_)				\
	for ((machine_device_) = &list[0];				\
	     (machine_device_) - &list[0] < ARRAY_SIZE(list);		\
	     (machine_device_)++)

#define for_each_device_event(machine_device_)				\
	for_each_device_state(machine_device_)				\
		if (!(machine_device_)->machine_cycle_event) continue; else

static bool valid_device_bus_address(u32 bus_address, const struct device *dev)
{
	return dev->bus.address <= bus_address &&
		bus_address < dev->bus.address + dev->bus.size;
}

const struct device *device_for_bus_address(u32 bus_address)
{
	const struct device *device;

	for_each_device (device)
		if (valid_device_bus_address(bus_address, device))
			return device;

	return &bus_device_error;
}

struct device_cycle device_cycle(const struct device *device)
{
	return (struct device_cycle) {
		.c = cycle_transform(device->frequency,
			MACHINE_FREQUENCY, machine_cycle())
	};
}

static struct device_cycle device_from_machine_cycle(
	const struct device *device, u64 machine_cycle)
{
	return (struct device_cycle) {
		.c = cycle_transform(device->frequency,
			MACHINE_FREQUENCY, machine_cycle)
	};
}

static struct device_slice device_from_machine_slice(
	const struct device *device, u64 machine_slice)
{
	return (struct device_slice) {
		.s = cycle_transform(device->frequency,
			MACHINE_FREQUENCY, machine_slice)
	};
}

static u64 machine_from_device_cycle(const struct device *device,
	const struct device_cycle device_cycle)
{
	return cycle_transform(MACHINE_FREQUENCY,
			device->frequency, device_cycle.c);
}

static u64 machine_from_device_slice(const struct device *device,
	const struct device_slice device_slice)
{
	return cycle_transform(MACHINE_FREQUENCY,
			device->frequency, device_slice.s);
}

void request_device_event(const struct device *device,
	struct device_cycle device_cycle)
{
	const u64 machine_cycle = machine_from_device_cycle(device, device_cycle);
	struct machine_device *machine_device = NULL;

	for (size_t i = 0; i < ARRAY_SIZE(list); i++)
		if (list[i].device == device) {
			machine_device = &list[i];
			break;
		}
	BUG_ON(!machine_device);

	// FIXME: printf("%s %s %lu\n", __func__, device->name, machine_cycle);

	if (!machine_device->machine_cycle_event ||
	    machine_cycle < machine_device->machine_cycle_event)
		machine_device->machine_cycle_event = machine_cycle;
}

void device_reset(void)
{
	const struct device *device;

	for_each_device (device)
		if (device->reset)
			device->reset(device);
}

static struct device_slice run(const struct device *device,
	u64 machine_cycle, u64 machine_slice)
{
	if (!device->run)
		return (struct device_slice) { };

	return device->run(device,
		device_from_machine_cycle(device, machine_cycle),
		device_from_machine_slice(device, machine_slice));
}

u64 device_run(u64 machine_cycle, u64 machine_slice)
{
	struct machine_device *machine_device;

	for_each_device_event (machine_device)
		if (machine_device->machine_cycle_event <= machine_cycle) {
			machine_device->machine_cycle_event = 0;

			machine_device->device->event(machine_device->device,
				device_from_machine_cycle(
					machine_device->device, machine_cycle));
		}

	for_each_device_event (machine_device)
		if (machine_device->machine_cycle_event <= machine_cycle)
			machine_slice = 0;
		else
			machine_slice = min(machine_slice,
				machine_device->machine_cycle_event - machine_cycle);

	if (!machine_slice)
		return 0;

	const struct device *device;

	for_each_device (device) {
		struct device_slice device_slice =
			run(device, machine_cycle, machine_slice);

		if (device_slice.s)
			return machine_from_device_slice(device, device_slice);
	}

	return machine_slice;
}
