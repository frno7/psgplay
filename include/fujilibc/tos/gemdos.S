// SPDX-License-Identifier: LGPL-2.1
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#ifndef _FUJILIBC_TOS_GEMDOS_S
#define _FUJILIBC_TOS_GEMDOS_S

#include <tos/stddef.S>

#include "trap.S"

#include "gemdos-call.h"

#define GEMDOS_OPCODE_EQU(opcode_, rtype_, name_, ptypes_, ...)		\
	.equ	gemdos_opcode_##name_,opcode_;
GEMDOS_CALL(GEMDOS_OPCODE_EQU)

.macro trap_gemdos opcode_ n_=0
.if \opcode_ == gemdos_opcode_pterm0
	clr.w	-(sp)
	trap	#1
.elseif \opcode_ == gemdos_opcode_pterm
	move.w	#\opcode_,-(sp)
	trap	#1
.else
	trap_opcode 1 \opcode_ \n_
.endif
.endm

#define GEMDOS_MACRO(opcode_, rtype_, name_, ptypes_, ...)		\
	.macro	trap_gemdos_##name_;					\
		trap_gemdos opcode_ TRAP_ARG_SIZE_##ptypes_;		\
	.endm;
GEMDOS_CALL(GEMDOS_MACRO)

#endif /* _FUJILIBC_TOS_GEMDOS_S */
