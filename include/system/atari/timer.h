// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#ifndef PSGPLAY_SYSTEM_ATARI_TIMER_H
#define PSGPLAY_SYSTEM_ATARI_TIMER_H

#include <tos/xbra.h>

#include "internal/types.h"

void timer_init(bool (*f)(uint32_t vector,
	struct xbra_regs *regs, void *arg), void *arg);

#endif /* PSGPLAY_SYSTEM_ATARI_TIMER_H */
