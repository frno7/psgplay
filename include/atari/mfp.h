// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#ifndef ATARI_MFP_H
#define ATARI_MFP_H

#include "bus.h"

u32 mfp_irq_vector(void);

void dma_sound_active(bool level);

extern const struct device mfp_device;

#endif /* ATARI_MFP_H */
