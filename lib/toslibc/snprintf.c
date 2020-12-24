// SPDX-License-Identifier: LGPL-2.1

#include <stdarg.h>
#include <stdio.h>

int snprintf(char *str, size_t size, const char *format, ...)
{
	va_list ap;

	va_start(ap, format);
	int length = vsnprintf(str, size, format, ap);
	va_end(ap);

	return length;
}
