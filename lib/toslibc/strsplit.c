// SPDX-License-Identifier: LGPL-2.1
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#include <stdbool.h>
#include <strsplit.h>

struct string_split _first_string_split(const char *s, const char *e,
	size_t (*sep)(const char *s, const char *e, void *arg), void *arg)
{
	if (s == e)
		return (struct string_split) {
			.length = 0,
			.s = s,
			.eos = true
		};

	const size_t t = sep(s, e, arg);

	if (t > 0)
		return (struct string_split) {
			.length = t,
			.s = s,
			.sep = true,
			.eos = (&s[t] == e)
		};

	size_t n = 1;
	while (&s[n] != e && !sep(&s[n], e, arg))
		n++;

	return (struct string_split) {
		.length = n,
		.s = s,
		.eos = (&s[n] == e)
	};
}

struct string_split _next_string_split(struct string_split split, const char *e,
	size_t (*sep)(const char *s, const char *e, void *arg), void *arg)
{
	return split.eos ? (struct string_split) { } :
		_first_string_split(&split.s[split.length], e, sep, arg);
}
