// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2020 Fredrik Noring
 */

#ifndef ATARI_SOUND_H
#define ATARI_SOUND_H

#include "atari/bus.h"
#include "atari/sample.h"

extern const struct device sound_device;

void sound_sample(sound_sample_f sample, void *sample_arg);

void record_sample(record_sample_f record, void *record_arg);

void sound_check(u32 bus_address);

#endif /* ATARI_SOUND_H */
