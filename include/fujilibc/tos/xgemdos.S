// SPDX-License-Identifier: LGPL-2.1
/*
 * Copyright (C) 2020 Fredrik Noring
 */

#ifndef _FUJILIBC_TOS_XGEMDOS_S
#define _FUJILIBC_TOS_XGEMDOS_S

#include <tos/stddef.S>

#include "trap.S"

#include "xgemdos-call.h"

#define XGEMDOS_OPCODE_EQU(opcode_, rtype_, name_, ...)			\
	.equ	xgemdos_opcode_##name_,opcode_;
XGEMDOS_CALL(XGEMDOS_OPCODE_EQU)

.macro trap_xgemdos opcode_
	move.w	#\opcode_,d0
	trap	#2
.endm

#define XGEMDOS_MACRO(opcode_, rtype_, name_, ...)			\
	.macro	trap_xgemdos_##name_;					\
		trap_xgemdos opcode_;					\
	.endm;
XGEMDOS_CALL(XGEMDOS_MACRO)

#endif /* _FUJILIBC_TOS_XGEMDOS_S */
