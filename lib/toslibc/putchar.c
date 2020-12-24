// SPDX-License-Identifier: LGPL-2.1
/*
 * Copyright (C) 2020 Fredrik Noring
 */

#include <stdio.h>
#include <unistd.h>

int putchar(int c)
{
	const unsigned char b = c;

	const ssize_t w = write(STDOUT_FILENO, &b, 1);

	return w == 1 ? b : EOF;
}
