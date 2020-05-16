// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#ifndef TOSLIBC_STRSPLIT_H
#define TOSLIBC_STRSPLIT_H

#if defined(_TOSLIBC_SOURCE)

#include <stdbool.h>
#include <stddef.h>

/**
 * struct string_split - string split cursor
 * @length: length of current substring, not necessarily NUL terminated
 * @s: pointer to current substring
 * @sep: %true if the current substring is the separator, otherwise %false
 * @eos: %true if the current substring is the last one, otherwise %false
 */
struct string_split {
	size_t length;
	const char *s;
	bool sep;
	bool eos;
};

/**
 * for_each_string_split - split a string by a separator
 * @split: string split cursor
 * @str: pointer to first character in string to split
 * @end: pointer succeeding last character in string to split
 * @sep: separator function
 * @arg: optional separator function argument, can be %NULL
 *
 * Note that all substrings including the separator is provided to the loop
 * statement. Hence, concatenating all substrings gives the original string.
 *
 * The loop statement is always invoked at least once, even when given the
 * empty string to split.
 */
#define for_each_string_split(split, str, end, sep, arg)		\
	for ((split) = _first_string_split((str), (end), (sep), (arg));	\
	     (split).s;							\
	     (split) = _next_string_split((split), (end), (sep), (arg)))

struct string_split _first_string_split(const char *s, const char *e,
	size_t (*sep)(const char *s, const char *e, void *arg), void *arg);

struct string_split _next_string_split(struct string_split split, const char *e,
	size_t (*sep)(const char *s, const char *e, void *arg), void *arg);

#endif /* defined(_TOSLIBC_SOURCE) */

#endif /* TOSLIBC_STRSPLIT_H */
