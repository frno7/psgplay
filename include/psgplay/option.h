// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#ifndef PSGPLAY_OPTION_H
#define PSGPLAY_OPTION_H

#include "psgplay/types.h"

#define OPTION_TIME_UNDEFINED -1
#define OPTION_STOP_NEVER     -2

struct options {
	int verbose;

	bool info;
	const char *output;

	const char *start;
	const char *length;
	const char *stop;

	int track;
	int frequency;

	const char *input;
};

int option_verbosity(void);

struct options *parse_options(int argc, char **argv);

#endif /* PSGPLAY_OPTION_H */
