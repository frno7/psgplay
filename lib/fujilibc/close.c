// SPDX-License-Identifier: LGPL-2.1
/*
 * Copyright (C) 2020 Fredrik Noring
 */

#include <errno.h>
#include <fcntl.h>
#include <tos/error.h>

#include <tos/gemdos.h>

int close(int fd)
{
	const int32_t err = gemdos_fclose(fd);

	if (err < 0) {
		errno = errno_for_tos_error(-err);

		return -1;
	}

	return 0;
}
