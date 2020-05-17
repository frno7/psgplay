// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#ifndef SYSTEM_UNIX_CLOCK_H
#define SYSTEM_UNIX_CLOCK_H

#include "internal/types.h"

void clock_init(void);

void clock_suspend(void);

void clock_resume(void);

void clock_update(void);

u32 clock_s(void);

u64 clock_ms(void);

void clock_request_s(const u32 timestamp);

void clock_request_ms(const u64 timestamp);

int clock_poll(void);

#endif /* SYSTEM_UNIX_CLOCK_H */
