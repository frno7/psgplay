// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#ifndef ATARI_CPU_H
#define ATARI_CPU_H

#include "atari/device.h"

void m68k_instruction_callback(int pc);

extern const struct device cpu_device;

#endif /* ATARI_CPU_H */
