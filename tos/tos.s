/* The Operating System (TOS) */

	.equ	TOS_VERSION, 0x0162	/* <major><minor> in BCD */
	.equ	TOS_RELEASE, 0x01011990	/* <month><day><year> in BCD */

	.equ	TOS_COUNTRY_US,  0	/* USA */
	.equ	TOS_COUNTRY_DE,  1	/* Germany */
	.equ	TOS_COUNTRY_FR,  2	/* France */
	.equ	TOS_COUNTRY_UK,  3	/* United Kingdom */
	.equ	TOS_COUNTRY_ES,  4	/* Spain */
	.equ	TOS_COUNTRY_IT,  5	/* Italy */
	.equ	TOS_COUNTRY_SE,  6	/* Sweden */
	.equ	TOS_COUNTRY_SF,  7	/* Switzerland (French) */
	.equ	TOS_COUNTRY_SG,  8	/* Switzerland (German) */
	.equ	TOS_COUNTRY_TR,  9	/* Turkey */
	.equ	TOS_COUNTRY_FI, 10	/* Finland */
	.equ	TOS_COUNTRY_NO, 11	/* Norway */
	.equ	TOS_COUNTRY_DK, 12	/* Denmark */
	.equ	TOS_COUNTRY_SA, 13	/* Saudi Arabia */
	.equ	TOS_COUNTRY_NL, 14	/* Holland */
	.equ	TOS_COUNTRY_CZ, 15	/* Czech Republic */
	.equ	TOS_COUNTRY_HU, 16	/* Hungary */
	.equ	TOS_COUNTRY_RU, 19	/* Russia */
	.equ	TOS_COUNTRY_GR, 31	/* Greece */

	.equ	TOS_NTSC, 0
	.equ	TOS_PAL,  1

	.equ	TOS_REGION_SE, (TOS_COUNTRY_SE << 1) | TOS_PAL

	.equ	_ev_vbl, 0x70		/* FIXME: Linker script */

	.equ	_mfp_vr, 0xfffffa17	/* FIXME: Linker script */

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
	bra.s	reset
	.dc.w	TOS_VERSION
	.dc.l	reset

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

reset:	lea	_stack_top,%sp	/* Set a valid stack pointer */
	move	#0x2700,%sr	/* Disable interrupts */
	move.b	#0x48,_mfp_vr	/* Set MFP exception vector base */

	move.l	#0x8,%a0
excopy:	move.l	#ignore,(%a0)+
	cmp.l	#0x13c,%a0
	bne.s	excopy

	move.l	#vbl,_ev_vbl

	jsr	0x1000		/* Call the init function */
loop:	stop	#0x2000		/* Enable and wait for interrupts */
	bra.s	loop

vbl:	jsr	0x1008		/* Call the play function */
	rte

ignore:	rte
