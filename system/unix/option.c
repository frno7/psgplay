// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#include <getopt.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "internal/build-assert.h"
#include "internal/compare.h"
#include "internal/types.h"

#include "psgplay/print.h"
#include "psgplay/version.h"

#include "system/unix/file.h"
#include "system/unix/option.h"

static struct options option;

int option_verbosity(void)
{
	return option.verbose;
}

static void help(FILE *file)
{
	fprintf(file,
"Usage: %s [options]... <sndh-file>\n"
"\n"
"General options:\n"
"\n"
"    -h, --help             display this help and exit\n"
"    --version              display version and exit\n"
"    -v, --verbose          increase verbosity\n"
"\n"
"    -i, --info             display SNDH file info and exit\n"
"    -o, --output=<file>    write audio output to the given file in WAVE format\n"
"\n"
"    --start=<[mm:]ss.ss>   start playing at the given time\n"
"    --stop=<[mm:]ss.ss|auto|never>\n"
"                           stop playing at the given time, or automatically\n"
"                           if the track has a known duration, or never\n"
"    --length=<[mm:]ss.ss>  play for the given duration\n"
"\n"
"    -t, --track=<num>      set track number\n"
"    -f, --frequency=<num>  set audio frequency in Hz (default 44100)\n"
"\n",
		progname);
}

static void NORETURN help_exit(int code)
{
	help(stdout);

	exit(code);
}

static void NORETURN version_exit(void)
{
	printf("%s version %s\n", progname, psgplay_version());

	exit(EXIT_SUCCESS);
}

struct options *parse_options(int argc, char **argv)
{
	static const struct option options[] = {
		{ "help",      no_argument,       NULL, 0 },
		{ "version",   no_argument,       NULL, 0 },
		{ "verbose",   no_argument,       NULL, 0 },

		{ "info",      no_argument,       NULL, 0 },
		{ "output",    required_argument, NULL, 0 },

		{ "start",     required_argument, NULL, 0 },
		{ "stop",      required_argument, NULL, 0 },
		{ "length",    required_argument, NULL, 0 },

		{ "track",     required_argument, NULL, 0 },
		{ "frequency", required_argument, NULL, 0 },

		{ NULL, 0, NULL, 0 }
	};

#define OPT(option) (strcmp(options[index].name, (option)) == 0)

	argv[0] = progname;	/* For better getopt_long error messages. */

	option.track = -1;
	option.frequency = 44100;

	for (;;) {
		int index = 0;

		switch (getopt_long(argc, argv, "hvit:f:o:", options, &index)) {
		case -1:
			if (optind == argc)
				help_exit(EXIT_FAILURE);
			goto out;

		case 0:
			if (OPT("help"))
				goto opt_h;
			else if (OPT("version"))
				version_exit();
			else if (OPT("verbose"))
				goto opt_v;

			else if (OPT("info"))
				goto opt_i;
			else if (OPT("output"))
				goto opt_o;

			else if (OPT("start"))
				option.start = optarg;
			else if (OPT("stop"))
				option.stop = optarg;
			else if (OPT("length"))
				option.length = optarg;

			else if (OPT("track"))
				goto opt_t;
			else if (OPT("frequency"))
				goto opt_f;
			break;

		case 'h':
opt_h:			help_exit(EXIT_SUCCESS);

		case 'v':
opt_v:			option.verbose++;
			break;

		case 'i':
opt_i:			option.info = true;
			break;

		case 'o':
opt_o:			option.output = optarg;
			break;

		case 't':
opt_t:			option.track = atoi(optarg);
			break;

		case 'f':
opt_f:			option.frequency = atoi(optarg);
			break;

		case '?':
			exit(EXIT_FAILURE);
		}
	}

#undef OPT
out:

	if (optind == argc)
		pr_fatal_error("missing input SNDH file\n");
	if (optind + 1 < argc)
		pr_fatal_error("%s: too many input SNDH files\n",
			argv[optind + 1]);

	option.input = argv[optind];

	return &option;
}
