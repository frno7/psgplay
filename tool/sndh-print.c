// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "psgplay/file.h"
#include "psgplay/print.h"
#include "psgplay/sndh-print.h"
#include "psgplay/string.h"

#include "sndh/sndh.h"

struct print_context {
	struct file file;

	int time_count;
	int subname_count;
	int subflag_count;
};

static bool printable(const char *s)
{
	for (size_t i = 0; s[i] != '\0'; i++) {
		int c = s[i] & 0xff;

		if (c < 0x20 || 0x7f == c)
			return false;
	}

	return true;
}

static void print_flags(const char *flags, struct file file)
{
	for (size_t i = 0; flags[i] != '\0'; i++)
		switch (flags[i]) {
#define PRINT_SNDH_FLAG(c, label)					\
		case c: printf(" " #label); break;
SNDH_FLAG(PRINT_SNDH_FLAG)
		default:
			pr_warn("%s: unrecognised flag '%c'\n",
				file.path, flags[i]);
		}
}

static bool print_tag(const char *name, const char *value, void *arg)
{
	struct print_context *context = arg;
	size_t length = strlen(value);

	if (!length)
		pr_warn("%s: tag %s empty\n", context->file.path, name);
	if (!printable(value))
		pr_warn("%s: tag %s contains nonprintable characters: %s\n",
			context->file.path, name, value);
	if (length && (isspace(value[0]) || isspace(value[length - 1])))
		pr_warn("%s: tag %s malformed whitespace\n",
			context->file.path, name);

	char *v = strrep(value, "\n", "\n\t");

	if (strcmp(name, "TIME") == 0) {
		printf("tag field %s %d %s\n",
			name, ++context->time_count, v);
	} else if (strcmp(name, "!#SN") == 0) {
		printf("tag field %s %d %s\n",
			name, ++context->subname_count, v);
	} else if (strcmp(name, "FLAG~") == 0) { /* FIXME: doc/sndhv21.txt */
		printf("tag field FLAG ~ %s", v);
		print_flags(v, context->file);
		printf("\n");
	} else if (strcmp(name, "FLAG") == 0) {
		printf("tag field %s %d %s",
			name, ++context->subflag_count, v);
		print_flags(v, context->file);
		printf("\n");
	} else
		printf("tag field %s%s%s\n",
			name, name[0] != '\0' ? " " : "", v);

	free(v);

	return true;
}

void sndh_print(struct file file)
{
	struct print_context context = {
		.file = file,
	};
	size_t tag_size;

	printf("path %s\n", file.path);

	if (sndh_tags(file, &tag_size, print_tag, &context))
		printf("tag size %zu valid\n", tag_size);
	else
		printf("tag size %zu invalid\n", tag_size);
	printf("data size %zu\n", file.size - tag_size);
}
