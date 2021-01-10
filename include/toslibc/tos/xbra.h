// SPDX-License-Identifier: LGPL-2.1
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#ifndef _TOSLIBC_TOS_XBRA_H
#define _TOSLIBC_TOS_XBRA_H

#include <stdbool.h>
#include <stdint.h>

#include <tos/bios.h>

/**
 * struct xbra_regs - XBRA registers
 * @d: the 8 processor data registers
 * @a: the 8 processor address registers
 * @usp: user stack pointer
 * @rte: return from exeception
 * @rte.sr: status register
 * @rte.pc: program counter
 * @sp: stack array in 16-bit words
 */
struct xbra_regs {
	int32_t d[8];
	void *a[8];
	void *usp;
	struct {
		uint16_t sr;
		void *pc;
	} __attribute__((__packed__)) rte;
	int16_t sp[];
} __attribute__((__packed__));

/**
 * typedef xbra_enter - XBRA enter function
 * @regs: registers before entering the previous exception vector
 * @arg: optional function argument supplied to ___xbra_init___()
 *
 * Return: %true to invoke the previous exception vector, otherwise %false
 */
typedef bool (*xbra_enter)(struct xbra_regs *regs, void *arg);

/**
 * typedef ___xbra_leave___ - XBRA leave function
 * @regs: registers after leaving the previous exception vector
 * @arg: optional function argument supplied to ___xbra_init___()
 */
typedef void (*xbra_leave)(struct xbra_regs *regs, void *arg);

/**
 * ___xbra_init___ - XBRA initialise and install exception vector
 * @vector: exception vector to install
 * @enter: function to invoke before entering the previous exception vector
 * @leave: function to invoke after leaving the previous exception vector
 * @arg: optional function argument, can be %NULL
 *
 * Note that ___xbra_exit___ must be called to uninstall the exception vector.
 */
void ___xbra_init___(uint32_t vector,
	xbra_enter enter, xbra_leave leave, void *arg);

/**
 * ___xbra_exit___ - XBRA uninstall exception vector
 */
void ___xbra_exit___(void);

#define DEFINE_XBRA(cookie_, label_) DEFINE_XBRA_REENTER(cookie_, label_, 16)

#define DEFINE_XBRA_REENTER(cookie_, label_, reenter_)			\
uint32_t __##label_##_vector;						\
void *__##label_##_arg;							\
struct __##label_##_rte { 						\
	uint16_t sr;							\
	void *pc;							\
} __attribute__((__packed__)) __##label_##_rte[reenter_];		\
struct __##label_##_rte *__##label_##_reenter = __##label_##_rte;	\
bool (*__##label_##_enter)(struct xbra_regs *regs, void *arg);		\
void (*__##label_##_leave)(struct xbra_regs *regs, void *arg);		\
extern void *__##label_##_org;						\
extern void __##label_(void);						\
									\
void label_##_init(uint32_t vector,					\
	xbra_enter enter, xbra_leave leave, void *arg)			\
{									\
	__##label_##_vector = vector;					\
	__##label_##_arg = arg;						\
	__##label_##_enter = enter;					\
	__##label_##_leave = leave;					\
	__##label_##_org = bios_setexc(vector, BIOS_SETEXC_INQUIRE);	\
	bios_setexc(vector, __##label_);				\
}									\
									\
void label_##_exit(void)						\
{									\
	__xbra_exit(__##label_##_vector, cookie_);			\
}									\
									\
__asm__ (								\
"	.pushsection .text.__"#label_"\n"				\
"	.even\n"							\
"	.ascii	\"XBRA\"\n"						\
"	.ascii	\""cookie_"\"\n"					\
"	.globl	__"#label_"_arg\n"					\
"	.globl	__"#label_"_reenter\n"					\
"	.globl	__"#label_"_enter\n"					\
"	.globl	__"#label_"_leave\n"					\
"	.globl	__"#label_"_org\n"					\
"__"#label_"_org:\n"							\
"	.dc.l	0\n"							\
"	.globl	__"#label_"\n"						\
"	.type	__"#label_",@function\n"				\
"__"#label_":\n"							\
	/* Check whether the installed enter function is NULL. */	\
"	tst.l	__"#label_"_enter\n"					\
"	beq.s	1f\n"							\
	/* Push the user stack pointer (USP). */			\
"	move.l	%a0,-8(%sp)\n"						\
"	move.l	%usp,%a0\n"						\
"	move.l	%a0,-(%sp)\n"						\
"	move.l	-4(%sp),%a0\n"						\
	/* Push the remaining arguments for the enter function. */	\
"	movem.l	%d0-%a7,-(%sp)\n"					\
"	addq.l	#4,15*4(%sp)\n"	/* Skip the USP pointer */		\
"	move.l	__"#label_"_arg,-(%sp)\n"				\
"	move.l	%sp,-(%sp)\n"						\
"	addq.l	#4,(%sp)\n"	/* Skip the arg pointer */		\
	/* JSR to the enter function. */				\
"	pea	0f(%pc)\n"						\
"	move.l	__"#label_"_enter,-(%sp)\n"				\
"	rts\n"								\
"0:\n"	/* Resume after the enter function. */				\
"	addq.l	#8,%sp\n"						\
	/* Check whether the enter function returned false. */		\
"	tst.l	%d0\n"							\
"	beq.w	4f\n"							\
"	movem.l	(%sp)+,%d0-%a7\n"					\
	/* Reinstall the user stack pointer (USP). */			\
"	move.l	%a0,-4(%sp)\n"						\
"	move.l	(%sp)+,%a0\n"						\
"	move.l	%a0,%usp\n"						\
"	move.l	-8(%sp),%a0\n"						\
"1:\n"									\
	/* Check whether the installed leave function is NULL. */	\
"	tst.l	__"#label_"_leave\n"					\
"	beq.w	5f\n"							\
	/* Save SR and PC for the original RTE. */			\
"	move.l	%a0,-4(%sp)\n"						\
"	move.l	__"#label_"_reenter,%a0\n"				\
"	move.w	(%sp),(%a0)\n"						\
"	move.l	2(%sp),2(%a0)\n"					\
"	move.l	-4(%sp),%a0\n"						\
	/* Increase level of reentrancy */				\
"	addq.l	#6,__"#label_"_reenter\n"				\
	/* Install our own SR and PC before invoking the exception. */	\
"	move.w	%sr,(%sp)\n"						\
"	move.l	#2f,2(%sp)\n"						\
	/* JMP to the original exception. */				\
"	move.l	__"#label_"_org,-(%sp)\n"				\
"	rts\n"								\
"2:\n"	/* Resume after the original exception RTE. */			\
	/* Decrease level of reentrancy. */				\
"	subq.l	#6,__"#label_"_reenter\n"				\
	/* Restore SR and PC for the original RTE. */			\
"	move.l	%a0,-4-6(%sp)\n"					\
"	move.l	__"#label_"_reenter,%a0\n"				\
"	move.l	2(%a0),-(%sp)\n"					\
"	move.w	(%a0),-(%sp)\n"						\
"	move.l	-4(%sp),%a0\n"						\
	/* Push the user stack pointer (USP). */			\
"	move.l	%a0,-8(%sp)\n"						\
"	move.l	%usp,%a0\n"						\
"	move.l	%a0,-(%sp)\n"						\
"	move.l	-4(%sp),%a0\n"						\
	/* Push the remaining arguments for the leave function. */	\
"	movem.l	%d0-%a7,-(%sp)\n"					\
"	addq.l	#4,15*4(%sp)\n"	/* Skip the USP pointer */		\
"	move.l	__"#label_"_arg,-(%sp)\n"				\
"	move.l	%sp,-(%sp)\n"						\
"	addq.l	#4,(%sp)\n"	/* Skip the arg pointer */		\
	/* JSR to the leave function. */				\
"	pea	3f(%pc)\n"						\
"	move.l	__"#label_"_leave,-(%sp)\n"				\
"	rts\n"								\
"3:\n"	/* Resume after the leave function. */				\
"	addq.l	#8,%sp\n"						\
"	movem.l	(%sp)+,%d0-%a7\n"					\
	/* Reinstall the user stack pointer (USP). */			\
"	move.l	%a0,-4(%sp)\n"						\
"	move.l	(%sp)+,%a0\n"						\
"	move.l	%a0,%usp\n"						\
"	move.l	-8(%sp),%a0\n"						\
"	rte\n"								\
"4:\n"									\
"	movem.l	(%sp)+,%d0-%a7\n"					\
	/* Reinstall the user stack pointer (USP). */			\
"	move.l	%a0,-4(%sp)\n"						\
"	move.l	(%sp)+,%a0\n"						\
"	move.l	%a0,%usp\n"						\
"	move.l	-8(%sp),%a0\n"						\
"	rte\n"								\
"5:\n"	/* JMP to the original exception. */				\
"	move.l	__"#label_"_org,-(%sp)\n"				\
"	rts\n"								\
"	.popsection\n"							\
)

void __xbra_exit(uint32_t vector, const char cookie[5]);

#endif /* _TOSLIBC_TOS_XBRA_H */
