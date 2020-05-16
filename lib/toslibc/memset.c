// SPDX-License-Identifier: GPL-2.0

#include <string.h>

void *memset(void *s, int c, size_t n)
{
	char *b = s;

	for (size_t i = 0; i < n; i++)
		b[i] = c;		/* FIXME: Optimise */

	return s;
}
