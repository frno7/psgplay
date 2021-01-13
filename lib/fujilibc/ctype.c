// SPDX-License-Identifier: LGPL-2.1

#include <ctype.h>

int isspace(int c)
{
	return c == ' ' ||
	       c == '\f' ||
	       c == '\n' ||
	       c == '\r' ||
	       c == '\t' ||
	       c == '\v';
}
