// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2025 Fredrik Noring
 */

#include <getopt.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "internal/build-assert.h"
#include "internal/compare.h"
#include "internal/print.h"
#include "internal/string.h"
#include "internal/types.h"

#include "test/option.h"

static struct options option;

int option_verbosity(void)
{
	return option.verbose;
}

static void help(FILE *file)
{
	fprintf(file,
"Usage: %s <command> [options]... <wave-file>\n"
"\n"
"General options:\n"
"\n"
"    -h, --help             display this help and exit\n"
"    -v, --verbose          increase verbosity\n"
"    --quiet                decrease verbosity\n"
"\n"
"    -o, --output=<file>    write output file\n"
"    -t, --track=<num>      set track number\n"
"\n",
		progname);
}

static void NORETURN help_exit(int code)
{
	help(stdout);

	exit(code);
}

static int track_from_path(const char *path)
{
	size_t k = 0;

	/* Find last '-' in path, for example in "test/tempo-123.wave". */
	for (size_t i = 0; path[i]; i++)
		if (path[i] == '-')
			k = i;
	if (path[k] != '-')
		return 0;

	const char *s = &path[k + 1];
	char *e;

	const unsigned long n = strtoul(s, &e, 10);
	if (s == e)
		return 0;

	return *e == '.' || *e == '\0' ? n : 0;
}

struct options *parse_options(int argc, char **argv)
{
	static const struct option options[] = {
		{ "help",                no_argument,       NULL, 0 },
		{ "verbose",             no_argument,       NULL, 0 },
		{ "quiet",               no_argument,       NULL, 0 },

		{ "output",              required_argument, NULL, 0 },
		{ "track",               required_argument, NULL, 0 },

		{ NULL, 0, NULL, 0 }
	};

#define OPT(option) (strcmp(options[index].name, (option)) == 0)

	progname = argv[0];

	for (;;) {
		int index = 0;

		switch (getopt_long(argc, argv,
			"hvt:o:", options, &index)) {
		case -1:
			goto out;

		case 0:
			if (OPT("help"))
				goto opt_h;
			else if (OPT("verbose"))
				goto opt_v;
			else if (OPT("quiet"))
				option.verbose--;

			else if (OPT("output"))
				goto opt_o;

			else if (OPT("track"))
				goto opt_t;
			break;

		case 'h':
opt_h:			help_exit(EXIT_SUCCESS);

		case 'v':
opt_v:			option.verbose++;
			break;

		case 'o':
opt_o:			option.output = optarg;
			break;

		case 't':
opt_t:			option.track = atoi(optarg);
			break;

		case '?':
			exit(EXIT_FAILURE);
		}
	}

#undef OPT
out:
	if (optind == argc)
		help_exit(EXIT_FAILURE);
	if (optind + 1 == argc)
		pr_fatal_error("missing input WAVE file\n");
	if (optind + 2 < argc)
		pr_fatal_error("%s: too many input files\n",
			argv[optind + 1]);

	option.command = argv[optind];
	option.input = argv[optind + 1];

	if (!option.track)
		option.track = track_from_path(option.input);
	if (!option.track)
		pr_fatal_error("%s: track not in file name and not given with --track\n",
			option.input);

	return &option;
}
