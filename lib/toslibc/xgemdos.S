// SPDX-License-Identifier: LGPL-2.1
/*
 * Copyright (C) 2020 Fredrik Noring
 */

#include <tos/xgemdos.S>

#define XGEMDOS_DEFINITION(opcode_, rtype_, name_, ...)			\
	.pushsection .text.xgemdos_##name_; 				\
	.globl	xgemdos_##name_;					\
	.type	xgemdos_##name_,@function;				\
xgemdos_##name_:;							\
	/* XGEMDOS clobbers d0-d1/a0 and C ABI needs d2-d7/a2-a6. */	\
	move.l	4(sp),d1;	/* AES and VDI take one argument. */	\
	trap_xgemdos opcode_;						\
	rts;								\
	.popsection;
XGEMDOS_CALL(XGEMDOS_DEFINITION)
