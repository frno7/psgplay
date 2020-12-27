// SPDX-License-Identifier: LGPL-2.1
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#include <stdlib.h>

#include "internal/macro.h"

#include "toslibc/tos/gemdos.h"

static void (*__atexit_list[8])(void);
static size_t __atexit_list_count;

void __attribute__((__noreturn__)) exit(int status)
{
	for (size_t i = 0; i < __atexit_list_count; i++)
		__atexit_list[__atexit_list_count - 1 - i]();

	gemdos_pterm(status);
}

int atexit(void (*function)(void))
{
	if (__atexit_list_count >= __ARRAY_SIZE(__atexit_list))
		return -1;

	__atexit_list[__atexit_list_count++] = function;

	return 0;
}
