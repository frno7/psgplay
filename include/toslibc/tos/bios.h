// SPDX-License-Identifier: LGPL-2.1
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#ifndef _TOSLIBC_TOS_BIOS_H
#define _TOSLIBC_TOS_BIOS_H

#include <stddef.h>
#include <stdint.h>

#include "internal/types.h"

#include "bios-call.h"

#define BIOS_SETEXC_INQUIRE ((void (*)(void))-1)

#define BIOS_DECALARE(opcode_, rtype_, name_, ptypes_, ...)		\
rtype_ bios_##name_(__VA_ARGS__);
BIOS_CALL(BIOS_DECALARE)

#endif /* _TOSLIBC_TOS_BIOS_H */
