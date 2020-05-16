// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2020 Fredrik Noring
 */

#include <errno.h>
#include <stdlib.h>

#include "toslibc/tos/gemdos.h"

void *malloc(size_t size)
{
	void *ptr = gemdos_malloc(size);

	if (!ptr && size)
		errno = -ENOMEM;

	return ptr;
}
