// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#include "atari/device.h"
#include "atari/fdc.h"

static void fdc_event(const struct device *device,
	const struct device_cycle mfp_cycle)
{
}

static void fdc_reset(const struct device *device)
{
}

const struct device fdc_device = {
	.name = "fdc",
	.frequency = 8000000,
	.reset = fdc_reset,
	.event = fdc_event,
};
