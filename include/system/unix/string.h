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

#endif /* PSGPLAY_SYSTEM_UNIX_STRING_H */
