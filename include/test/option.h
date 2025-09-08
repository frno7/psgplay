// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2025 Fredrik Noring
 */

#ifndef PSGPLAY_TEST_OPTION_H
#define PSGPLAY_TEST_OPTION_H

#include <stdbool.h>

extern const char *progname;

struct options {
	char name[64];

	int verbose;

	const char *command;
	const char *input;
	const char *output;

	int track;
};

int option_verbosity(void);

int track_from_path(const char *path);

void name_from_input();

struct options *parse_options(int argc, char **argv);

#endif /* PSGPLAY_TEST_OPTION_H */
