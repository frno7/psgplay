// SPDX-License-Identifier: LGPL-2.1
/*
 * Copyright (C) 2020 Fredrik Noring
 */

#include <errno.h>
#include <tos/error.h>
#include <unistd.h>

#include <tos/gemdos.h>

ssize_t write(int fd, const void *buf, size_t count)
{
	const ssize_t r = gemdos_fwrite(fd, count, buf);

	if (r < 0) {
		errno = errno_for_tos_error(-r);

		return -1;
	}

	return r;
}
