// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#ifndef ATARI_PSG_H
#define ATARI_PSG_H

#include "toslibc/asm/machine.h"

#include "atari/bus.h"
#include "atari/sample.h"

#define PSG_FREQUENCY	(ATARI_STE_EXT_OSC / ATARI_STE_SND_PSG_DIV)

extern const struct device psg_device;

void psg_sample(psg_sample_f sample, void *sample_arg);

#endif /* ATARI_PSG_H */
