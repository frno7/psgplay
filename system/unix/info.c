// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "psgplay/print.h"
#include "psgplay/info.h"
#include "psgplay/string.h"

#include "sndh/sndh.h"

#include "unicode/atari.h"

#include "system/unix/file.h"

static void print_flags(const char *flags, struct file file)
{
	for (size_t i = 0; flags[i] != '\0'; i++)
		switch (flags[i]) {
#define PRINT_SNDH_FLAG(c, label, description)				\
		case c: printf(" " #label); break;
SNDH_FLAG(PRINT_SNDH_FLAG)
		default:
			pr_warn("%s: unrecognised flag '%c'\n",
				file.path, flags[i]);
		}
}

void sndh_print(struct file file)
{
	size_t header_size;

	int time_count = 0;
	int subname_count = 0;
	int subflag_count = 0;

	printf("path %s\n", file.path);

	sndh_for_each_tag_with_header_size (file.data, file.size, &header_size) {
		const char *name = sndh_tag_name;
		const char *value = sndh_tag_value;
		const size_t length = strlen(value);

		u8 *u = charset_to_utf8_string((const u8 *)value, length,
				charset_atari_st_to_utf32, NULL);

		char *v = strrep((const char *)u, "\n", "\n\t");

		if (strcmp(name, "TIME") == 0) {
			printf("tag field %s %d %s\n",
				name, ++time_count, v);
		} else if (strcmp(name, "!#SN") == 0) {
			printf("tag field %s %d %s\n",
				name, ++subname_count, v);
		} else if (strcmp(name, "FLAG~") == 0) {
			/* FIXME: Document FLAG~ and FLAG in doc/sndhv21.txt */
			printf("tag field FLAG ~ %s", v);
			print_flags(v, file);
			printf("\n");
		} else if (strcmp(name, "FLAG") == 0) {
			printf("tag field %s %d %s",
				name, ++subflag_count, v);
			print_flags(v, file);
			printf("\n");
		} else
			printf("tag field %s%s%s\n",
				name, name[0] != '\0' ? " " : "", v);

		free(v);
		free(u);
	}

	printf("data size %zu\n", file.size - header_size);
}
