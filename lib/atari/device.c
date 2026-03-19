// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#include "internal/compare.h"
#include "internal/struct.h"
#include "internal/types.h"

#include "atari/bus.h"
#include "atari/cpu.h"
#include "atari/fdc.h"
#include "atari/glue.h"
#include "atari/machine.h"
#include "atari/mixer.h"
#include "atari/mfp.h"
#include "atari/psg.h"
#include "atari/ram.h"
#include "atari/rom.h"
#include "atari/shifter.h"
#include "atari/sound.h"

#include "m68k/m68kcpu.h"

#define for_each_device(list, device_)					\
	for (struct machine_device *machine_device_ = &(list)->d[0];	\
	     (machine_device_) - &(list)->d[0] < ARRAY_SIZE((list)->d) && \
	     ((device_) = machine_device_->device);			\
	     (machine_device_)++)

#define for_each_device_state(list, machine_device_)			\
	for ((machine_device_) = &(list)->d[0];				\
	     (machine_device_) - &(list)->d[0] < ARRAY_SIZE((list)->d);	\
	     (machine_device_)++)

#define for_each_device_event(list, machine_device_)			\
	for_each_device_state((list), machine_device_)			\
		if (!(machine_device_)->machine_cycle_event) continue; else

bool valid_device_bus_address(u32 bus_address, const struct device *dev)
{
	return dev->bus.address <= bus_address &&
		bus_address < dev->bus.address + dev->bus.size;
}

const struct device *device_for_bus_address(struct machine *machine,
	u32 bus_address)
{
	struct machine_device_list *list = &machine->device.list;
	const struct device *device;

	for_each_device (list, device)
		if (valid_device_bus_address(bus_address, device))
			return device;

	return &bus_device_error;
}

struct device_cycle device_cycle(struct machine *machine,
	const struct device *device)
{
	return (struct device_cycle) {
		.c = cycle_transform(device->clk.frequency / device->clk.divisor,
			CPU_FREQUENCY, machine_cycle(machine))
	};
}

struct device_cycle device_from_machine_cycle(
	const struct device *device, u64 machine_cycle)
{
	return (struct device_cycle) {
		.c = cycle_transform(device->clk.frequency / device->clk.divisor,
			CPU_FREQUENCY, machine_cycle)
	};
}

static struct device_slice device_from_machine_slice(
	const struct device *device, u64 machine_slice)
{
	return (struct device_slice) {
		.s = cycle_transform(device->clk.frequency / device->clk.divisor,
			CPU_FREQUENCY, machine_slice)
	};
}

static u64 machine_from_device_cycle_align(const struct device *device,
	const struct device_cycle device_cycle)
{
	return cycle_transform_align(CPU_FREQUENCY,
			device->clk.frequency / device->clk.divisor, device_cycle.c);
}

static u64 machine_from_device_slice(const struct device *device,
	const struct device_slice device_slice)
{
	return cycle_transform(CPU_FREQUENCY,
			device->clk.frequency / device->clk.divisor, device_slice.s);
}

void request_device_event(struct machine *machine,
	const struct device *device, struct device_cycle device_cycle)
{
	struct machine_device_list *list = &machine->device.list;
	const u64 machine_cycle =
		machine_from_device_cycle_align(device, device_cycle);
	struct machine_device *machine_device = NULL;

	if (machine->device.device_run_cycle.machine_slice_end &&
	    machine->device.device_run_cycle.machine_slice_end > machine_cycle)
		m68k_end_timeslice(&musashi_module);

	for (size_t i = 0; i < ARRAY_SIZE(list->d); i++)
		if (list->d[i].device == device) {
			machine_device = &list->d[i];
			break;
		}
#if 0  /* FIXME: Dependency on pr_bug */
	BUG_ON(!machine_device);
#endif

	if (!machine_device->machine_cycle_event ||
	    machine_cycle < machine_device->machine_cycle_event)
		machine_device->machine_cycle_event = machine_cycle;
}

void device_reset(struct machine *machine)
{
	struct machine_device_list *list = &machine->device.list;
	const struct device *device;

	*list = (struct machine_device_list) {
		{
			{ .device = &rom_device     },
			{ .device = &glue_device    },
			{ .device = &ram_device     },
			{ .device = &mfp_device     },
			{ .device = &shifter_device },
			{ .device = &psg_device     },
			{ .device = &sound_device   },
			{ .device = &mixer_device   },
			{ .device = &fdc_device     },
			{ .device = &cpu_device     },
		}
	};

	for_each_device (list, device)
		if (device->reset)
			device->reset(machine, device);
}

static struct device_slice run(struct machine *machine,
	const struct device *device, u64 machine_cycle, u64 machine_slice)
{
	if (!device->run)
		return (struct device_slice) { };

	machine->device.device_run_cycle = (struct device_run_cycle) {
		.machine_slice_end = machine_cycle + machine_slice,
	};

	const struct device_slice slice =
		device->run(machine, device,
			device_from_machine_cycle(device, machine_cycle),
			device_from_machine_slice(device, machine_slice));

	machine->device.device_run_cycle = (struct device_run_cycle) { };

	return slice;
}

u64 device_run(struct machine *machine, u64 machine_cycle, u64 machine_slice)
{
	struct machine_device_list *list = &machine->device.list;
	struct machine_device *machine_device;

	for_each_device_event (list, machine_device)
		if (machine_device->machine_cycle_event <= machine_cycle) {
			machine_device->machine_cycle_event = 0;

			machine_device->device->event(machine,
				machine_device->device,
				device_from_machine_cycle(
					machine_device->device, machine_cycle));
		}

	for_each_device_event (list, machine_device)
		if (machine_device->machine_cycle_event <= machine_cycle)
			machine_slice = 0;
		else
			machine_slice = min(machine_slice,
				machine_device->machine_cycle_event - machine_cycle);

	if (!machine_slice)
		return 0;

	const struct device *device;

	for_each_device (list, device) {
		struct device_slice device_slice =
			run(machine, device, machine_cycle, machine_slice);

		if (device_slice.s)
			return machine_from_device_slice(device, device_slice);
	}

	return machine_slice;
}
