// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2020 Fredrik Noring
 */

#include <errno.h>
#include <tos/error.h>
#include <unistd.h>

#include "toslibc/tos/gemdos.h"

off_t lseek(int fd, off_t offset, int whence)
{
	const off_t r = gemdos_fseek(offset, fd, whence);

	if (r < 0) {
		errno = errno_for_tos_error(-r);

		return -1;
	}

	return r;
}
