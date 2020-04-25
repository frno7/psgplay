// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#include "psgplay/types.h"

static struct record {
	size_t size;
	struct {
		u32 init;
		u32 exit;
		u32 play;
		u8 data[];
	} *sndh;
} record;

static void sndh_init(int track)
{
	__asm__ __volatile__ (
		"	movem.l	%%d0-%%a6,-(%%sp)\n"
		"	move.l	%1,%%d0\n"
		"	jsr	(%0)\n"
		"	movem.l	(%%sp)+,%%d0-%%a6\n"
		:
		: "a" (&record.sndh->init), "d" (track)
		: "memory");
}

static void sndh_play(void)
{
	__asm__ __volatile__ (
		"	movem.l	%%d0-%%a6,-(%%sp)\n"
		"	jsr	(%0)\n"
		"	movem.l	(%%sp)+,%%d0-%%a6\n"
		:
		: "a" (&record.sndh->play)
		: "memory");
}

void vbl_exception(void)
{
	sndh_play();
}

void timer_a_exception(void) { }
void timer_b_exception(void) { }
void timer_c_exception(void) { }
void timer_d_exception(void) { }

static void idle(void)
{
	__asm__ __volatile__ ("stop #0x2200" : : : "cc");
}

static void idle_indefinitely(void)
{
	for (;;)
		idle();
}

void start(size_t size, void *sndh, u32 track, u32 timer)
{
	record = (struct record) { .size = size, .sndh = sndh };

	sndh_init(track);

	idle_indefinitely();
}
