// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#ifndef ATARI_MFP_H
#define ATARI_MFP_H

#include "bus.h"

uint32_t mfp_irq_vector(struct machine *machine);

void dma_sound_active(struct machine *machine, bool level);

extern const struct device mfp_device;

#endif /* ATARI_MFP_H */
