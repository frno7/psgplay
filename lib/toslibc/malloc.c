// SPDX-License-Identifier: LGPL-2.1
/*
 * Copyright (C) 2020 Fredrik Noring
 */

#include <errno.h>
#include <stdlib.h>

#include <tos/gemdos.h>

void *malloc(size_t size)
{
	if (!size)
		return NULL;

	void *ptr = gemdos_malloc(size);

	if (!ptr)
		errno = -ENOMEM;

	return ptr;
}
