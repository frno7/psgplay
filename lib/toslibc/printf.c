// SPDX-License-Identifier: GPL-2.0

#include <stdarg.h>
#include <stdio.h>

int printf(const char *format, ...)
{
	va_list ap;

	va_start(ap, format);
	int length = vprintf(format, ap);
	va_end(ap);

	return length;
}
