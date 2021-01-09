// SPDX-License-Identifier: LGPL-2.1
/*
 * Copyright (C) 2020 Fredrik Noring
 */

#ifndef _TOSLIBC_TOS_XGEMDOS_H
#define _TOSLIBC_TOS_XGEMDOS_H

#include <stddef.h>
#include <stdint.h>

#include "xgemdos-call.h"

#define XGEMDOS_DECALARE(opcode_, rtype_, name_, ...)			\
rtype_ xgemdos_##name_(__VA_ARGS__);
XGEMDOS_CALL(XGEMDOS_DECALARE)

#endif /* _TOSLIBC_TOS_XGEMDOS_H */
