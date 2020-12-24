// SPDX-License-Identifier: LGPL-2.1
/*
 * Copyright (C) 2020 Fredrik Noring
 */

#include <errno.h>
#include <fcntl.h>
#include <tos/error.h>

#include "toslibc/tos/gemdos.h"

int open(const char *pathname, int flags)
{
	const int32_t fd = gemdos_fopen(pathname, flags);

	if (fd < 0) {
		errno = errno_for_tos_error(-fd);

		return -1;
	}

	return fd;
}
