// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2020 Fredrik Noring
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>

int puts(const char *s)
{
	const size_t length = strlen(s);
	const ssize_t w = write(STDOUT_FILENO, s, length);

	return w == length ? w : EOF;
}
