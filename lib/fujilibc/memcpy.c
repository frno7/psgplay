// SPDX-License-Identifier: LGPL-2.1

#include <string.h>

void *memcpy(void *dst, const void *src, size_t nbytes)
{
	const char *s = src;
	char *d = dst;

	for (size_t i = 0; i < nbytes; i++)
		d[i] = s[i];		/* FIXME: Optimise */

	return dst;
}
