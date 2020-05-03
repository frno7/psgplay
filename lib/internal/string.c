// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#include <errno.h>
#include <limits.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "internal/string.h"

bool strtoint(int *n, const char *s, int base)
{
	char *e;

	errno = 0;
	const long int m = strtol(s, &e, base);

	if (e != &s[strlen(s)] || errno == ERANGE)
		return false;

	if (m < INT_MIN || INT_MAX < m)
		return false;

	*n = m;

	return true;
}
