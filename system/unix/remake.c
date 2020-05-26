// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#include <ctype.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "internal/print.h"

#include "psgplay/sndh.h"

#include "system/unix/memory.h"
#include "system/unix/remake.h"
#include "system/unix/string.h"

static char *escape(const char *s)
{
	const size_t len = strlen(s);
	char *e = xmalloc(4 * len + 1); /* "\xXX" is the longest expansion */
	size_t i, j;

	for (i = j = 0; s[i] != '\0'; i++)
		if (s[i] == '\t') {
			e[j++] = '\\';
			e[j++] = 't';
		} else if (s[i] == '\r') {
			e[j++] = '\\';
			e[j++] = 'r';
		} else if (s[i] == '\n') {
			e[j++] = '\\';
			e[j++] = 'n';
		} else if (s[i] == '\\' || s[i] == '"') {
			e[j++] = '\\';
			e[j++] = s[i];
		} else if (isprint(s[i])) {
			e[j++] = s[i];
		} else
			j += sprintf(&e[j], "\\x%02x", (uint8_t)s[i]);

	e[j] = '\0';

	return e;
}

static const char *trim(char *s)
{
	return s;

	const size_t len = strlen(s);

	if (!len)
		return s;

	for (char *e = &s[len - 1]; e != s; e--)
		if (isspace(*e))
			*e = '\0';
		else
			break;

	while (isspace(*s))
		s++;

	return s;
}

static void print_substrings_tag(const char *tag,
	const char *label, const char *prefix,
	const void *data, size_t size)
{
	int subtune_count;

	printf("\t.even\n%s:\n\t.ascii\t\"%s\"\n", label, tag);

	if (!sndh_tag_subtune_count(&subtune_count, data, size))
		subtune_count = 1;

	for (int i = 0; i < subtune_count; i++)
		printf("\t.dc.w\t%s%d-%s\n", prefix, 1 + i, label);
}

void remake_header(const void *data, size_t size)
{
	printf("\t.ascii\t\"SNDH\"\n");

	int time_count = 0;
	int subtitle_count = 0;
	int subflag_count = 0;

	sndh_for_each_tag (data, size) {
		const char *name = sndh_tag_name;
		char *value = escape(sndh_tag_value);
		const char *v = trim(value);

		if (strcmp(name, "TIME") == 0) {
			if (!time_count++)
				printf("\t.even\n\t.ascii\t\"TIME\"\n");
			printf("\t.dc.w\t\%d\n", sndh_tag_integer);
		} else if (strcmp(name, "!#SN") == 0) {
			if (!subtitle_count++)
				print_substrings_tag(name,
					".subtitles", ".st", data, size);
			printf(".st%d:\t.asciz\t\"%s\"\n", subtitle_count, v);
		} else if (strcmp(name, "FLAG") == 0) {
			if (!subflag_count++)
				print_substrings_tag(name,
					".subflags", ".sf", data, size);
			printf(".sf%d:\t.asciz\t\"%s\"\n", subflag_count, v);
		} else if (strcmp(name, "##") == 0) {
			printf("\t.asciz\t\"%s%02d\"\n", name, sndh_tag_integer);
		} else
			printf("\t.asciz\t\"%s%s\"\n", name, v);

		free(value);
	}

	printf("\t.even\n");
	printf("\t.ascii\t\"HDNS\"\n");
}
