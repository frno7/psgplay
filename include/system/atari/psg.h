// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#ifndef PSGPLAY_SYSTEM_ATARI_PSG_H
#define PSGPLAY_SYSTEM_ATARI_PSG_H

#include "internal/types.h"

void psg_init(void);

uint8_t psg_mute(void);

void psg_unmute(uint8_t iomix);

#endif /* PSGPLAY_SYSTEM_ATARI_PSG_H */
