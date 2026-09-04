// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#include <ctype.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "internal/print.h"

#include "psgplay/sndh.h"

#include "system/unix/file.h"
#include "system/unix/info.h"
#include "system/unix/string.h"

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

static void print_frms(const char *name,
	int frms, int subtune, struct file file)
{
	float duration = 0.0f;

	if (sndh_tag_subtune_time(&duration, subtune, file.data, file.size)) {
		const int d = roundf(duration);
		const int seconds = d % 60;
		const int minutes = d / 60;

		printf("tag field %s %-2d %6d %8.3f  %02d:%02d\n",
			name, subtune, frms, duration, minutes, seconds);
	} else
		printf("tag field %s %d %d - -\n", name, subtune, frms);
}

void sndh_print(struct file file)
{
	size_t header_size;

	int frms_count = 0;
	int time_count = 0;
	int subname_count = 0;
	int subflag_count = 0;

	printf("path %s\n", file.path);

	sndh_for_each_tag_with_header_size (file.data, file.size, &header_size) {
		const char *name = sndh_tag_name;
		const char *text = sndh_tag_text;

		char *v = strrep(text, "\n", "\n\t");

		if (strcmp(name, "FRMS") == 0) {
			print_frms(name, sndh_tag_integer, ++frms_count, file);
		} else if (strcmp(name, "TIME") == 0) {
			const int seconds = sndh_tag_integer % 60;
			const int minutes = sndh_tag_integer / 60;

			printf("tag field %s %d %d %02d:%02d\n",
				name, ++time_count, sndh_tag_integer,
				minutes, seconds);
		} else if (strcmp(name, "!#SN") == 0) {
			printf("tag field %s %d %s\n",
				name, ++subname_count, v);
		} else if (strcmp(name, "FLAG~") == 0) {
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
	}

	printf("header size %zu\n", header_size);
	printf("data size %zu\n", file.size - header_size);
}
