// SPDX-License-Identifier: LGPL-2.1

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
