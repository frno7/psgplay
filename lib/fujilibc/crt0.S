// SPDX-License-Identifier: LGPL-2.1
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#include "tos/gemdos.S"

	.equ	stack_size, 0x2000

	.text

	.globl	_start
	.type	_start,@function
_start:	move.l	4(sp),a0	/* Basepage address */
	move.l	a0,_basepage
	lea	_stack,sp

	move.l	0x18(a0),d0	/* BSS base address */
	add.l	0x1c(a0),d0	/* Add BSS size */
	sub.l	a0,d0		/* Transient program area (TPA) size */

	move.l	d0,-(sp)
	move.l	a0,-(sp)
	clr.w	-(sp)
	trap_gemdos_mshrink

	tst.l	d0
	beq	_main
	move.w	d0,-(sp)
	trap_gemdos_pterm

	.bss

	.globl	_stack
	.ds.b	stack_size
	.even
_stack:

	.globl	_basepage
_basepage:
	.ds.l	1
