// SPDX-License-Identifier: GPL-2.0

#ifndef _TOSLIBC_STDIO_H
#define _TOSLIBC_STDIO_H

#include <stdarg.h>
#include <stddef.h>

#define EOF (-1)

int dprintf(int fd, const char *format, ...)
	__attribute__((format(printf, 2, 3)));

int printf(const char *format, ...)
	__attribute__((format(printf, 1, 2)));

int putchar(int c);

int puts(const char *s);

int vdprintf(int fd, const char *format, va_list ap);

int vprintf(const char *format, va_list ap);

int vsnprintf(char *str, size_t size, const char *format, va_list ap);

int snprintf(char *str, size_t size, const char *format, ...)
	__attribute__((format(printf, 3, 4)));

#endif /* _TOSLIBC_STDIO_H */
