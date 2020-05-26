// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#define _POSIX_C_SOURCE 199309
#include <time.h>

#include "internal/compare.h"
#include "internal/types.h"
#include "internal/print.h"

#include "system/unix/clock.h"

static u64 start;
static u64 suspend;
static u64 offset;
static u64 now;

static u64 request;

static u64 clock_now(void)
{
	struct timespec tp;

	if (clock_gettime(CLOCK_MONOTONIC, &tp) == -1)
		pr_fatal_errno("clock_now:clock_gettime");

	return tp.tv_sec * 1000000000 + tp.tv_nsec;
}

void clock_init(void)
{
	start = clock_now();
}

void clock_suspend(void)
{
	suspend = clock_now();
}

void clock_resume(void)
{
	offset += clock_now() - suspend;
}

void clock_update(void)
{
	now = clock_now() - start - offset;
}

u32 clock_s(void)
{
	return clock_ms() / 1000;
}

u64 clock_ms(void)
{
	return now / 1000000;
}

void clock_request_s(const u32 timestamp)
{
	if (!timestamp)
		return;

	clock_request_ms(1000 * timestamp);
}

void clock_request_ms(const u64 timestamp)
{
	if (!timestamp)
		return;

	const u64 t = timestamp * 1000000;

	request = !request ? t : min(request, t);
}

int clock_poll(void)
{
	if (!request)
		return -1;

	const u64 r = request;

	request = 0;

	if (r <= now)
		return 0;

	return (r - now) / 1000000;
}
