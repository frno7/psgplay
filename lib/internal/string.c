// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#include <errno.h>
#include <limits.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "internal/assert.h"
#include "internal/string.h"
#include "internal/types.h"

bool strtoint(int *n, const char *s, int base)
{
	char *e;

	errno = 0;
	const long int m = strtol(s, &e, base);

	if (e != &s[strlen(s)] || errno == ERANGE)
		return false;

	if (m < INT_MIN || INT_MAX < m)
		return false;

	*n = m;

	return true;
}

struct string_split first_string_split(
	const char *s, const char *sep,
	char *(find)(const char *s, const char *sep))
{
	if (s[0] == '\0')
		return (struct string_split) {
			.length = 0,
			.s = s,
			.eos = true
		};

	if (sep[0] == '\0')
		return (struct string_split) {
			.length = 1,
			.s = s,
			.eos = (s[1] == '\0')
		};

	const char *t = find(s, sep);

	if (!t)
		return (struct string_split) {
			.length = strlen(s),
			.s = s,
			.eos = true
		};

	const size_t n = t - s;

	if (!n) {
		const size_t sep_len = strlen(sep);

		return (struct string_split) {
			.length = sep_len,
			.s = s,
			.sep = true,
			.eos = (s[sep_len] == '\0')
		};
	}

	return (struct string_split) {
		.length = n,
		.s = s,
		.eos = (s[n] == '\0')
	};
}

struct string_split next_string_split(
	struct string_split split, const char *sep,
	char *(find)(const char *s, const char *sep))
{
	return split.eos ? (struct string_split) { } :
		first_string_split(&split.s[split.length], sep, find);
}

struct line_column char_line_column(char c, struct line_column lc)
{
	if (c == '\n') {
		lc.line++;
		lc.column = 1;
	} else
		lc.column = 1 + (c == '\t' ?
			(((lc.column - 1) >> 3) << 3) + 8 : lc.column);

	return lc;
}

struct line_column string_line_column(const char *s, struct line_column lc)
{
	for (size_t i = 0; s[i] != '\0'; i++)
		lc = char_line_column(s[i], lc);

	return lc;
}
