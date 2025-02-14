// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#ifndef PSGPLAY_SYSTEM_UNIX_OPTION_H
#define PSGPLAY_SYSTEM_UNIX_OPTION_H

#include <stdbool.h>

#include "atari/trace.h"
#include "psgplay/stereo.h"

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
	const char *stop;
	const char *length;

	const char *mode;

	int track;
	int frequency;

	const char *psg_mix;
	struct psgplay_psg_stereo_balance psg_balance;

	const char *input;

	struct trace_mode trace;
	enum disassemble_type disassemble;
	bool disassemble_address;
	bool remake_header;
};

int option_verbosity(void);

bool command_mode_option(void);

bool text_mode_option(void);

psgplay_digital_to_stereo_cb psg_mix_option(void);
void *psg_mix_arg(void);

struct options *parse_options(int argc, char **argv);

#endif /* PSGPLAY_SYSTEM_UNIX_OPTION_H */
