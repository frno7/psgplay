// SPDX-License-Identifier: LGPL-2.1

#include <stdarg.h>
#include <stdio.h>

int dprintf(int fd, const char *format, ...)
{
	va_list ap;

	va_start(ap, format);
	int length = vdprintf(fd, format, ap);
	va_end(ap);

	return length;
}
