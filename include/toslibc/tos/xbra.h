// SPDX-License-Identifier: LGPL-2.1
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#ifndef _TOSLIBC_TOS_XBRA_H
#define _TOSLIBC_TOS_XBRA_H

#include <stdbool.h>
#include <stdint.h>

#include <tos/bios.h>

#include "internal/build-assert.h"

#define DEFINE_XBRA(cookie_, label_)					\
uint32_t __##label_##_vector;						\
void *__##label_##_arg;							\
void (*__##label_##_f)(void *arg);					\
extern void *__##label_##_old;						\
extern void __##label_(void);						\
									\
void label_##_init(uint32_t vector, void (*f)(void *arg), void *arg)	\
{									\
	BUILD_BUG_ON(sizeof(cookie_) != 5); /* 4 char string + NUL */	\
	__##label_##_vector = vector;					\
	__##label_##_arg = arg;						\
	__##label_##_f = f;						\
	__##label_##_old = bios_setexc(vector, BIOS_SETEXC_INQUIRE);	\
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
"	.globl	__"#label_"_old\n"					\
"__"#label_"_old:\n"							\
"	.dc.l	0\n"							\
"	.globl	__"#label_"\n"						\
"	.type	__"#label_",@function\n"				\
"__"#label_":\n"							\
"	movem.l	%d0-%a6,-(%sp)\n"					\
"	move.l	__"#label_"_arg,-(%sp)\n"				\
"	move.l	__"#label_"_f,%a0\n"					\
"	jsr	(%a0)\n"						\
"	addq.l	#4,%sp\n"						\
"	movem.l	(%sp)+,%d0-%a6\n"					\
"	move.l	__"#label_"_old,-(%sp)\n"				\
"	rts\n"								\
"	.popsection\n"							\
)

void __xbra_exit(u32 vector, const char cookie[5]);

#endif /* _TOSLIBC_TOS_XBRA_H */
