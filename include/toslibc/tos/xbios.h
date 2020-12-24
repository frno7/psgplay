// SPDX-License-Identifier: LGPL-2.1
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#ifndef _TOSLIBC_TOS_XBIOS_H
#define _TOSLIBC_TOS_XBIOS_H

#include <stddef.h>
#include <stdint.h>

#include "internal/types.h"

#include "xbios-call.h"

#define XBIOS_GIACCESS_SET 0x80

#define XBIOS_DECALARE(opcode_, rtype_, name_, ptypes_, ...)		\
rtype_ xbios_##name_(__VA_ARGS__);
XBIOS_CALL(XBIOS_DECALARE)

/**
 * xbios_supexecarg - variant of xbios_supexec with argument
 * @func: function to be called in supervisor mode
 * @arg: argument to function
 */
void xbios_supexecarg(void (*func)(void *arg), void *arg);

#endif /* _TOSLIBC_TOS_XBIOS_H */
