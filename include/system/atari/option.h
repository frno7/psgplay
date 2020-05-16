// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#ifndef PSGPLAY_SYSTEM_ATARI_OPTION_H
#define PSGPLAY_SYSTEM_ATARI_OPTION_H

extern char progname[];

struct options {
	const char *input;
};

struct options *parse_options(int argc, char **argv);

#endif /* PSGPLAY_SYSTEM_ATARI_OPTION_H */
