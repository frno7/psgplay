// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#ifndef _TOSLIBC_TOS_BIOS_S
#define _TOSLIBC_TOS_BIOS_S

#include <tos/stddef.S>

#include "trap.S"

#include "bios-call.h"

#define BIOS_OPCODE_EQU(opcode_, rtype_, name_, ptypes_, ...)		\
	.equ	bios_opcode_##name_,opcode_;
BIOS_CALL(BIOS_OPCODE_EQU)

.macro trap_bios opcode_ n_=0
	trap_opcode 13 \opcode_ \n_
.endm

#define BIOS_MACRO(opcode_, rtype_, name_, ptypes_, ...)		\
	.macro	trap_bios_##name_;					\
		trap_bios opcode_ TRAP_ARG_SIZE_##ptypes_;		\
	.endm;
BIOS_CALL(BIOS_MACRO)

#endif /* _TOSLIBC_TOS_BIOS_S */
