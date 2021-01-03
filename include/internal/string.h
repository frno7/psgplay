// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#ifndef INTERNAL_STRING_H
#define INTERNAL_STRING_H

#include <stdbool.h>

bool strtoint(int *n, const char *s, int base);

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
#define for_each_string_split(split, str, sep)				\
	for ((split) = first_string_split((str), (sep), strstr);	\
	     (split).s;							\
	     (split) = next_string_split((split), (sep), strstr))

struct line_column {
	int line;
	int column;
};

/**
 * char_line_column - update line and column given a character
 * @c: character for update
 * @lc: line and column to update
 *
 * Return: line and column after character update
 */
struct line_column char_line_column(char c, struct line_column lc);

#endif /* INTERNAL_STRING_H */
