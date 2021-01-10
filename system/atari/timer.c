// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2020 Fredrik Noring
 */

#include <stdlib.h>

#include <tos/xbra.h>

DEFINE_XBRA("PSGP", timer_c);

void timer_init(bool (*f)(uint32_t vector,
	struct xbra_regs *regs, void *arg), void *arg)
{
	atexit(timer_c_exit);

	timer_c_init(69, f, NULL, arg);
}
