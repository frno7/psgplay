// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#ifndef ATARI_IRQ_H
#define ATARI_IRQ_H

#include "m68k/m68k.h"

#include "atari/bus.h"

#define IRQ_HBL M68K_IRQ_2
#define IRQ_VBL M68K_IRQ_4
#define IRQ_MFP M68K_IRQ_6

#endif /* ATARI_IRQ_H */
