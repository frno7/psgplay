// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#include <tos/xbios.S>

#define XBIOS_DEFINITION(opcode_, rtype_, name_, ptypes_, ...)		\
	.pushsection .text.xbios_##name_; 				\
	.globl	xbios_##name_;						\
	.type	xbios_##name_,@function;				\
xbios_##name_:;								\
	/* XBIOS clobbers d0-d2/a0-a2 and C ABI needs d2-d7/a2-a6. */	\
	move.l	d2,-(sp);						\
	move.l	a2,-(sp);						\
	trap_push_args_##ptypes_ 8;	/* 8=2*4 for d2/a2 pushed. */	\
	trap_xbios opcode_ TRAP_ARG_SIZE_##ptypes_;			\
	move.l	d0,a0;	/* Must have a0 for pointer return types. */	\
	move.l	(sp)+,a2; 						\
	move.l	(sp)+,d2; 						\
	rts;								\
	.popsection;
XBIOS_CALL(XBIOS_DEFINITION)
