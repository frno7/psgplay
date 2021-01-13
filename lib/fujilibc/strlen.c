// SPDX-License-Identifier: LGPL-2.1

#include <string.h>

size_t strlen(const char *s)
{
	const char *b = s;

	while (*s)
		s++;

	return s - b;
}
