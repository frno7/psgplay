// SPDX-License-Identifier: LGPL-2.1
/*
 * Copyright (C) 2020 Fredrik Noring
 */

#include <errno.h>

char *strerror(int errnum)
{
	switch (errnum) {
#define __ERRNO_CASE(symbol_, number_, description_)			\
	case symbol_: return description_;
__ERRNO_LIST(__ERRNO_CASE)
	default: return "Unknown error";
	}
}
