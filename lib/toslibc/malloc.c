// SPDX-License-Identifier: LGPL-2.1
/*
 * Copyright (C) 2020 Fredrik Noring
 */

#include <errno.h>
#include <stdlib.h>

#include <tos/gemdos.h>

#include "internal/malloc.h"

void *malloc(size_t size)
{
	if (!size)
		return NULL;

	struct __malloc *m = gemdos_malloc(sizeof(struct __malloc) + size);

	if (!m)
		errno = -ENOMEM;

	m->size = size;

	return &m[1];
}
