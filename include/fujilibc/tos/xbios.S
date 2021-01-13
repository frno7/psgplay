// SPDX-License-Identifier: LGPL-2.1
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#ifndef _FUJILIBC_TOS_XBIOS_S
#define _FUJILIBC_TOS_XBIOS_S

#include <tos/stddef.S>

#include "trap.S"

#include "xbios-call.h"

#define XBIOS_OPCODE_EQU(opcode_, rtype_, name_, ptypes_, ...)		\
	.equ	xbios_opcode_##name_,opcode_;
XBIOS_CALL(XBIOS_OPCODE_EQU)

.macro trap_xbios opcode_ n_=0
	trap_opcode 14 \opcode_ \n_
.endm

#define XBIOS_MACRO(opcode_, rtype_, name_, ptypes_, ...)		\
	.macro	trap_xbios_##name_;					\
		trap_xbios opcode_ TRAP_ARG_SIZE_##ptypes_;		\
	.endm;
XBIOS_CALL(XBIOS_MACRO)

#endif /* _FUJILIBC_TOS_XBIOS_S */
