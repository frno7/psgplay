// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#ifndef PSGPLAY_SYSTEM_UNIX_OPTION_H
#define PSGPLAY_SYSTEM_UNIX_OPTION_H

#include <stdbool.h>

#define OPTION_TIME_UNDEFINED -1
#define OPTION_STOP_NEVER     -2

extern char progname[];

enum disassemble_type {
	DISASSEMBLE_TYPE_NONE,
	DISASSEMBLE_TYPE_ALL,
	DISASSEMBLE_TYPE_HEADER,
};

struct options {
	int verbose;

	bool info;
	const char *output;

	const char *start;
	const char *length;
	const char *stop;

	const char *mode;

	int track;
	int frequency;

	const char *input;

	enum disassemble_type disassemble;
};

int option_verbosity(void);

bool command_mode_option(void);

bool text_mode_option(void);

struct options *parse_options(int argc, char **argv);

int option_verbosity(void);

#endif /* PSGPLAY_SYSTEM_UNIX_OPTION_H */
