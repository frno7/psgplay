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
#include "internal/print.h"
#include "internal/types.h"

#include "psgplay/version.h"

#include "system/unix/file.h"
#include "system/unix/option.h"
#include "system/unix/tty.h"

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
"\n"
"Play options:\n"
"\n"
"    -o, --output=<file>    write audio output to the given file in WAVE format\n"
"\n"
"    --start=<[mm:]ss.ss>   start playing at the given time\n"
"    --stop=<[mm:]ss.ss|auto|never>\n"
"                           stop playing at the given time, or automatically\n"
"                           if the track has a known duration, or never\n"
"    --length=<[mm:]ss.ss>  play for the given duration\n"
"\n"
"    -m, --mode=<command|text>\n"
"                           command or interactive text mode\n"
"\n"
"    -t, --track=<num>      set track number\n"
"    -f, --frequency=<num>  set audio frequency in Hz (default 44100)\n"
"\n"
"Disassembly options:\n"
"\n"
"    --disassemble          disassemble SNDH file and exit\n"
"    --disassemble-header   disassemble SNDH file header and exit\n"
"    --remake-header        remake SNDH file header in disassembly\n"
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

static bool command_mode_only_option(void)
{
	return option.verbose ||
	       option.info    ||
	       option.output  ||
	       option.start   ||
	       option.length  ||
	       option.stop;
}

bool command_mode_option(void)
{
	if (option.mode)
		return strcmp(option.mode, "command") == 0;

	return command_mode_only_option();
}

bool text_mode_option(void)
{
	if (option.mode) {
		if (strcmp(option.mode, "text") != 0)
			return false;

		if (command_mode_only_option())
			pr_fatal_error("command options in text mode\n");

		if (!tty_present())
			pr_fatal_error("text mode requires a tty\n");

		return true;
	}

	if (command_mode_only_option())
		return false;

	if (!tty_present())
		return false;

	return true;
}

struct options *parse_options(int argc, char **argv)
{
	static const struct option options[] = {
		{ "help",               no_argument,       NULL, 0 },
		{ "version",            no_argument,       NULL, 0 },
		{ "verbose",            no_argument,       NULL, 0 },

		{ "info",               no_argument,       NULL, 0 },
		{ "output",             required_argument, NULL, 0 },

		{ "start",              required_argument, NULL, 0 },
		{ "stop",               required_argument, NULL, 0 },
		{ "length",             required_argument, NULL, 0 },

		{ "mode",               required_argument, NULL, 0 },

		{ "track",              required_argument, NULL, 0 },
		{ "frequency",          required_argument, NULL, 0 },

		{ "disassemble",        no_argument,       NULL, 0 },
		{ "disassemble-header", no_argument,       NULL, 0 },
		{ "remake-header",      no_argument,       NULL, 0 },

		{ NULL, 0, NULL, 0 }
	};

#define OPT(option) (strcmp(options[index].name, (option)) == 0)

	argv[0] = progname;	/* For better getopt_long error messages. */

	option.track = -1;
	option.frequency = 44100;

	for (;;) {
		int index = 0;

		switch (getopt_long(argc, argv,
			"hvim:t:f:o:", options, &index)) {
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

			else if (OPT("mode"))
				goto opt_m;

			else if (OPT("track"))
				goto opt_t;
			else if (OPT("frequency"))
				goto opt_f;

			else if (OPT("disassemble"))
				option.disassemble = DISASSEMBLE_TYPE_ALL;
			else if (OPT("disassemble-header"))
				option.disassemble = DISASSEMBLE_TYPE_HEADER;
			else if (OPT("remake-header"))
				option.remake_header = true;
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

		case 'm':
opt_m:			option.mode = optarg;
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
