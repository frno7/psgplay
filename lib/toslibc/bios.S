// SPDX-License-Identifier: LGPL-2.1
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#include <tos/bios.S>

#define BIOS_DEFINITION(opcode_, rtype_, name_, ptypes_, ...)		\
	.pushsection .text.bios_##name_; 				\
	.globl	bios_##name_;						\
	.type	bios_##name_,@function;					\
bios_##name_:;								\
	/* BIOS clobbers d0-d2/a0-a2 and C ABI needs d2-d7/a2-a6. */	\
	move.l	d2,-(sp);						\
	move.l	a2,-(sp);						\
	trap_push_args_##ptypes_ 8;	/* 8=2*4 for d2/a2 pushed. */	\
	trap_bios opcode_ TRAP_ARG_SIZE_##ptypes_;			\
	move.l	d0,a0;	/* Must have a0 for pointer return types. */	\
	move.l	(sp)+,a2; 						\
	move.l	(sp)+,d2; 						\
	rts;								\
	.popsection;
BIOS_CALL(BIOS_DEFINITION)
