// SPDX-License-Identifier: LGPL-2.1

#ifndef _FUJILIBC_STDDEF_H
#define _FUJILIBC_STDDEF_H

#include <stdint.h>

#define NULL ((void *)0)

typedef __SIZE_TYPE__ size_t;
typedef __INT32_TYPE__ ssize_t;
#if __SIZEOF_SIZE_T__ != 4
#error "unable to define ssize_t"
#endif

#define __NORETURN __attribute__((__noreturn__))

#endif /* _FUJILIBC_STDDEF_H */
