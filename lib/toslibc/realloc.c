// SPDX-License-Identifier: LGPL-2.1
/*
 * Copyright (C) 2021 Fredrik Noring
 */

#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "internal/compare.h"
#include "internal/malloc.h"

void *realloc(void *ptr, size_t size)
{
	if (!size) {
		free(ptr);

		return NULL;
	}

	void *p = malloc(size);
	if (!p)
		return NULL;

	if (ptr) {
		struct __malloc *m0 = ptr;

		memcpy(p, ptr, min(m0[-1].size, size));

		free(ptr);
	}

	return p;
}
