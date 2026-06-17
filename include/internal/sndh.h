// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#ifndef INTERNAL_SNDH_H
#define INTERNAL_SNDH_H

#include "psgplay/sndh.h"

#if defined(__m68k__)

static inline void sndh_init(int track, struct sndh_file *file)
{
	__asm__ __volatile__ (
		"	movem.l	%%d0-%%a6,-(%%sp)\n"
		"	move.l	%1,%%d0\n"
		"	jsr	(%0)\n"
		"	movem.l	(%%sp)+,%%d0-%%a6\n"
		:
		: "a" (&file->sndh->init), "d" (track)
		: "memory");
}

static inline void sndh_exit(struct sndh_file *file)
{
	__asm__ __volatile__ (
		"	movem.l	%%d0-%%a6,-(%%sp)\n"
		"	jsr	(%0)\n"
		"	movem.l	(%%sp)+,%%d0-%%a6\n"
		:
		: "a" (&file->sndh->exit)
		: "memory");
}

static inline void sndh_play(struct sndh_file *file)
{
	__asm__ __volatile__ (
		/* Enable interrupts and other timers for effects. */
		"	move	#0x2200,%%sr\n"
		"	movem.l	%%d0-%%a6,-(%%sp)\n"
		"	jsr	(%0)\n"
		"	movem.l	(%%sp)+,%%d0-%%a6\n"
		:
		: "a" (&file->sndh->play)
		: "memory");
}

#endif /* defined(__m68k__) */

#endif /* INTERNAL_SNDH_H */
