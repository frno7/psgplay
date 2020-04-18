// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "psgplay/assert.h"
#include "psgplay/memory.h"
#include "psgplay/print.h"
#include "psgplay/string.h"
#include "psgplay/types.h"

char *xstrdup(const char *s)
{
	void *t = strdup(s);

	if (!t)
		pr_fatal_errno("strdup");

	return t;
}

char *xstrcat(const char *a, const char *b)
{
	const size_t alen = strlen(a);
	const size_t blen = strlen(b);
	char *s = xmalloc(alen + blen + 1);

	memcpy(&s[0], a, alen);
	memcpy(&s[alen], b, blen);
	s[alen + blen] = '\0';

	return s;
}

char *xstrndup(const char *s, size_t n)
{
	void *t = strndup(s, n);

	if (!t)
		pr_fatal_errno("strndup");

	return t;
}

bool strtoint(int *n, const char *s, int base)
{
	char *e;

	errno = 0;
	*n = strtoll(s, &e, base);

	return e == &s[strlen(s)] && errno != ERANGE;
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

char *strrep(const char *s, const char *from, const char *to)
{
	const size_t s_len = strlen(s);
	const size_t from_len = strlen(from);
	const size_t to_len = strlen(to);
	struct string_split split;

	size_t n = 0;
	for_each_string_split (split, s, from)
		if (split.sep)
			n++;

	const size_t t_len = to_len < from_len ?
		s_len - (from_len - to_len) * n :
		s_len + (to_len - from_len) * n;

	char *t = xmalloc(t_len + 1);

	size_t i = 0;
	for_each_string_split (split, s, from)
		if (split.sep) {
			memcpy(&t[i], to, to_len);
			i += to_len;
		} else {
			memcpy(&t[i], split.s, split.length);
			i += split.length;
		}

	BUG_ON(i != t_len);

	t[i] = '\0';

	return t;
}
