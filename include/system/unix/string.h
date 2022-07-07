// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#ifndef PSGPLAY_SYSTEM_UNIX_STRING_H
#define PSGPLAY_SYSTEM_UNIX_STRING_H

#include <stdbool.h>

char *xstrdup(const char *s);

char *xstrcat(const char *a, const char *b);

char *xstrndup(const char *s, size_t n);

/**
 * strrep - substitute each matching substring in a given string
 * @s: string to substitute substrings in
 * @from: substring to match
 * @to: replacement substring
 *
 * Return: allocated substituted string
 */
char *strrep(const char *s, const char *from, const char *to);	/* FIXME */

/**
 * struct strbuf - string buffer
 * @length: length in bytes of @s, excluding any terminating NUL
 * @capacity: maximum size in bytes of @s
 * @data: contents of string, NUL terminated only if @size > 0
 */
struct strbuf {
	size_t length;
	size_t capacity;
	char *s;
};

void sbfree(struct strbuf *sb);

/**
 * sbprintf - formatted output conversion to a string buffer
 * @sb: string buffer, can be initialised to zero
 * @fmt: a printf family format
 * @...: parameters to @fmt
 *
 * Return: %true if successful, otherwise %false
 */
bool sbprintf(struct strbuf *sb, const char *fmt, ...);

bool sbmprintf(struct strbuf *sb, size_t margin, const char *fmt, ...);

bool vsbprintf(struct strbuf *sb, const char *fmt, va_list ap);

bool vsbmprintf(struct strbuf *sb, size_t margin, const char *fmt, va_list ap);

#endif /* PSGPLAY_SYSTEM_UNIX_STRING_H */
