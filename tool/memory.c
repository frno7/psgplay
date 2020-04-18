// SPDX-License-Identifier: GPL-2.0

#include <stdlib.h>
#include <string.h>

#include "psgplay/memory.h"
#include "psgplay/print.h"
#include "psgplay/types.h"

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
