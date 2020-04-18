// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#ifndef ATARI_GLUE_H
#define ATARI_GLUE_H

#include "atari/device.h"

void glue_irq_set(int irq);
void glue_irq_clr(int irq);

extern const struct device glue_device;

#endif /* ATARI_GLUE_H */
