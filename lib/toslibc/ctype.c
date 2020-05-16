// SPDX-License-Identifier: GPL-2.0

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
