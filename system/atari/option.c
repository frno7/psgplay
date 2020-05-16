// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <tos/gemdos.h>

#include "internal/macro.h"

#include "system/atari/option.h"

static struct options option;

static void help(void)
{
	printf("Usage: %s <sndh-file>\r\n", progname);
}

static void NORETURN help_exit(int code)
{
	help();

	gemdos_cconin();

	exit(code);
}

struct options *parse_options(int argc, char **argv)
{
	if (argc != 2 || argv[1][0] == '-')
		help_exit(EXIT_FAILURE);

	option.input = argv[1];

	return &option;
}
