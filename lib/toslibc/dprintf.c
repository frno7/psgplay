// SPDX-License-Identifier: GPL-2.0

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
