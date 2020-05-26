// SPDX-License-Identifier: GPL-2.0

#include <stdlib.h>
#include <string.h>

#include "internal/print.h"
#include "internal/types.h"

#include "system/unix/memory.h"

void *xmalloc(size_t size)
{
	void *p = malloc(size);

	if (!p)
		pr_fatal_errno("malloc");

	return p;
}

void *zalloc(size_t size)
{
	void *p = xmalloc(size);

	memset(p, 0, size);

	return p;
}

void *xrealloc(void *ptr, size_t size)
{
	void *p = realloc(ptr, size);

	if (!p)
		pr_fatal_errno("realloc");

	return p;
}

void *xmemdup(const void *ptr, size_t size)
{
	void *p = xmalloc(size);

	memcpy(p, ptr, size);

	return p;
}
