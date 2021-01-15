// SPDX-License-Identifier: LGPL-2.1

#include <stdarg.h>
#include <stdio.h>
#include <unistd.h>

int vprintf(const char *format, va_list ap)
{
	return vdprintf(STDOUT_FILENO, format, ap);
}
