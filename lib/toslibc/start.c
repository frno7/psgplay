// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <strsplit.h>

#include "internal/build-assert.h"

#include "tos/basepage.h"

#define for_each_cmd_split(split)					\
	for_each_string_split((split),					\
		_basepage->cmd,						\
		&_basepage->cmd[_basepage->cmdlen],			\
		sepspace, NULL) if ((split).sep) continue; else

static size_t sepspace(const char *s, const char *e, void *arg)
{
	size_t n = 0;

	while (&s[n] != e && isspace(s[n]))
		n++;

	return n;
}

void _main(void)
{
	int main(int argc, char *argv[]);
	struct string_split split;

	BUILD_BUG_ON(sizeof(*_basepage) != 256);

	size_t argc = 1;
	size_t size = 0;

	if (_basepage->cmdlen > 0) {
		for_each_cmd_split(split) {
			argc++;
			size += split.length + 1;
		}
	}

	char argv0[] = "";	/* FIXME: Program filepath */
	char *argv[argc + 1];
	char buffer[size];

	argc = 0;
	size = 0;
	argv[argc++] = argv0;

	if (_basepage->cmdlen > 0) {
		for_each_cmd_split(split) {
			memcpy(&buffer[size], split.s, split.length);
			buffer[size + split.length] = '\0';

			argv[argc++] = &buffer[size];
			size += split.length + 1;
		}
	}

	argv[argc] = NULL;

	exit(main(argc, argv));
}

