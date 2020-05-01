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

struct string_split first_string_split(
	const char *s, const char *sep,
	char *(find)(const char *s, const char *sep));

struct string_split next_string_split(
	struct string_split split, const char *sep,
	char *(find)(const char *s, const char *sep));

/**
 * for_each_string_split - split a string by a separator
 * @split: string split cursor
 * @str: string to split
 * @sep: separator string
 *
 * Note that all substrings including the separator is provided to the loop
 * statement. Hence, concatenating all substrings gives the original string.
 *
 * The loop statement is always invoked at least once, even when given the
 * empty string to split.
 *
 * Splitting on the empty separator splits every character.
 */
#define for_each_string_split(split, str, sep)	/* FIXME */		\
	for ((split) = first_string_split((str), (sep), strstr);	\
	     (split).s;							\
	     (split) = next_string_split((split), (sep), strstr))

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
