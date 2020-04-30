/* The Operating System (TOS) */

#include <tos/stddef.S>

#include "tos/region.h"

	.equ	TOS_VERSION, 0x0162	/* <major><minor> in BCD */
	.equ	TOS_RELEASE, 0x01011990	/* <month><day><year> in BCD */

	.equ	TOS_REGION_SE, (TOS_COUNTRY_SE << 1) | TOS_PAL

	.text

	/*
	 * The first 8 bytes of the ROM are mirrored at address zero:
	 *
	 *  $000 reset: initial supervisor stack pointer (SSP)
	 *  $004 reset: initial program counter (PC)
	 *
	 *  The SSP is repurposed to contain a bra.s instruction and
	 *  the TOS version.
	 */
	.globl	_rom
_rom:	bra.s	.reset
	.dc.w	TOS_VERSION
	.dc.l	.reset

	.dc.l	0		/* FIXME: Base of OS */
	.dc.l	0		/* FIXME */
	.dc.l	0		/* Reserved */
	.dc.l	0		/* FIXME */
	.dc.l	TOS_RELEASE
	.dc.w	TOS_REGION_SE
	.dc.w	0		/* FIXME: TOS release date in GEMDOS format */
	.dc.l	0		/* FIXME */
	.dc.l	0		/* FIXME */
	.dc.l	0		/* FIXME */
	.dc.l	0		/* FIXME */

.reset:	move	#0x2700,sr	/* Disable interrupts */
	lea	_stack_top,sp	/* Set a valid stack pointer */
	move.b	#0x48,0xfffa17	/* Set MFP exception vector base */

	/* Ignore all exceptions by default */
	move.l	#0x8,a1
.loop:	move.l	#.ignore,(a1)+
	cmp.l	#0x13c,a1
	bne.s	.loop

	/* Install VBL and timer A, B, C and D vectors */
	move.l	#.vbl,0x70
	move.l	#.timer_a,0x134
	move.l	#.timer_b,0x120
	move.l	#.timer_c,0x114
	move.l	#.timer_d,0x110

	move.l	d2,-(sp)	/* u32 timer */
	move.l	d1,-(sp)	/* u32 track */
	move.l	a0,-(sp)	/* void *sndh */
	move.l	d0,-(sp)	/* size_t size */
	jsr	start
	sub.l	#16,sp

.halt:	stop	#0x2700
	bra.s	.halt

.ignore:
	rte

.macro	exception	name
.\name:	movem.l	d0-a6,-(sp)
	jsr	\name\()_exception
	movem.l	(sp)+,d0-a6
	rte
.endm

	exception vbl
	exception timer_a
	exception timer_b
	exception timer_c
	exception timer_d
