// SPDX-License-Identifier: LGPL-2.1

#include <string.h>

void *memset(void *s, int c, size_t n)
{
	char *b = s;

	for (size_t i = 0; i < n; i++)
		b[i] = c;		/* FIXME: Optimise */

	return s;
}
