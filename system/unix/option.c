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
#include "internal/string.h"
#include "internal/types.h"

#include "atari/trace.h"

#include "psgplay/version.h"

#include "audio/alsa-writer.h"

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
"    --quiet                decrease verbosity\n"
"\n"
"    -i, --info             display SNDH file info and exit\n"
"\n"
"Play options:\n"
"\n"
"    -o, --output=<file>    write audio output to the file in WAVE format"
#ifdef HAVE_ALSA
"\n"
"                           or to an ALSA handle if prefixed with \"alsa:\".\n"
#else
".\n"
#endif /* HAVE_ALSA */
"                           See Notes below on post-processing audio\n"
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
"    --psg-mix=<empiric|linear>\n"
"                           empiric (default) mixes the three PSG channels\n"
"                           as measured on Atari ST hardware, having a lofi,\n"
"                           crunchy, zesty quality; linear sums the channels\n"
"                           to produce a cleaner, somewhat insipid sound. See\n"
"                           Notes below on combining filters\n"
"    --psg-balance=<A:B:C>  set balance between -1 (left) and +1 (right) for\n"
"                           PSG channels A, B and C. For example -0.4:0:+0.4\n"
"                           for stereo effect. Default is 0:0:0 for mono. A\n"
"                           linear mix of PSG channels is forced. See Notes\n"
"                           below on combining filters\n"
"    --psg-volume=<A:B:C>   set volume between 0 (off) and +1 (max) for\n"
"                           PSG channels A, B and C. For example 0:0:1 to\n"
"                           play channel C only. Default is 1:1:1. See Notes\n"
"                           below on combining filters\n"
"\n"
"Disassembly options:\n"
"\n"
"    --disassemble          disassemble SNDH file and exit; may be combined\n"
"                           with the --trace=cpu option for self-modifying code,\n"
"                           disassembly of interrupt code, etc.\n"
"    --disassemble-header   disassemble SNDH file header and exit\n"
"    --disassemble-address  display address column in disassembly\n"
"    --remake-header        remake SNDH file header in disassembly\n"
"\n"
"Trace options:\n"
"\n"
"    --trace=<device>,...   trace device operations of SNDH file and exit:\n"
#define TRACE_DEVICE_HELP(symbol_, label_, id_) " " #symbol_
"                          " TRACE_DEVICE(TRACE_DEVICE_HELP) "\n"
"\n"
"Notes:\n"
"\n"
"PSG play does not yet completely emulate the Atari STE LMC1992 tone (bass and\n"
"treble) hardware.\n"
"\n"
"Multiple filters using the --psg-mix, --psg-balance, or --psg-volume options\n"
"cannot be combined. The last option given replaces any previous ones.\n"
"\n"
"PSG play audio has a DC offset for optimum audio quality, because YM2149 chip\n"
"music hardware, made in the 1970s, fundamentally generates a unipolar signal\n"
"which is converted into a bipolar signal. It may be necessary to remove this\n"
"DC offset with a 2-5 Hz high-pass filter before post-processing.\n"
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

static bool file_output(void)
{
#ifdef HAVE_ALSA
	if (alsa_writer_handle(option.output))
		return false;
#endif /* HAVE_ALSA */

	return option.output != NULL;
}

static bool command_mode_only_option(void)
{
	return option.verbose ||
	       option.info    ||
	       option.start   ||
	       option.length  ||
	       option.stop    ||
	       file_output();
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

static struct psgplay_psg_stereo_balance psg_balance_option(const char *s)
{
	struct string_split b;
	float abc[3] = { };
	size_t i = 0;

	for_each_string_split (b, s, ":") {
		if (b.sep)
			continue;
		if (i >= ARRAY_SIZE(abc))
			pr_fatal_error("too many balances: %s\n", s);

		char *e;
		abc[i++] = strtof(b.s, &e);

		if (e != &b.s[b.length])
			pr_fatal_error("invalid balance: %.*s\n",
				(int)b.length, b.s);
	}

	if (i != ARRAY_SIZE(abc))
		pr_fatal_error("too few balances: %s\n", s);

	return (struct psgplay_psg_stereo_balance) {
		.a = abc[0],
		.b = abc[1],
		.c = abc[2],
	};
}

static struct psgplay_psg_stereo_volume psg_volume_option(const char *s)
{
	struct string_split b;
	float abc[3] = { };
	size_t i = 0;

	for_each_string_split (b, s, ":") {
		if (b.sep)
			continue;
		if (i >= ARRAY_SIZE(abc))
			pr_fatal_error("too many volumes: %s\n", s);

		char *e;
		abc[i++] = strtof(b.s, &e);

		if (e != &b.s[b.length])
			pr_fatal_error("invalid volume: %.*s\n",
				(int)b.length, b.s);
	}

	if (i != ARRAY_SIZE(abc))
		pr_fatal_error("too few volumes: %s\n", s);

	return (struct psgplay_psg_stereo_volume) {
		.a = abc[0],
		.b = abc[1],
		.c = abc[2],
	};
}

static struct trace_mode trace_option(const char *s)
{
	struct string_split dev;
	uint32_t m = 0;

#define TRACE_DEVICE_OPT(symbol_, label_, id_)				\
	else if (strncmp(dev.s, #symbol_, dev.length) == 0 &&		\
			(#symbol_)[dev.length] == '\0')			\
		m |= TRACE_DEVICE_##label_;

	for_each_string_split (dev, s, ",")
		if (dev.sep)
			continue;
TRACE_DEVICE(TRACE_DEVICE_OPT)
		else
			pr_fatal_error("unknown device: %.*s\n",
				(int)dev.length, dev.s);

	return (struct trace_mode) { .m = m };
}

psgplay_digital_to_stereo_cb psg_mix_option(void)
{
	if (strcmp(option.psg_mix, "empiric") == 0)
		return psgplay_digital_to_stereo_empiric;
	if (strcmp(option.psg_mix, "linear") == 0)
		return psgplay_digital_to_stereo_linear;
	if (strcmp(option.psg_mix, "balance") == 0)
		return psgplay_digital_to_stereo_balance;
	if (strcmp(option.psg_mix, "volume") == 0)
		return psgplay_digital_to_stereo_volume;

	pr_fatal_error("unknown PSG mix: %s\n", option.psg_mix);

	return NULL;
}

void *psg_mix_arg(void)
{
	if (strcmp(option.psg_mix, "balance") == 0)
		return &option.psg_balance;
	if (strcmp(option.psg_mix, "volume") == 0)
		return &option.psg_volume;

	return NULL;
}

static void set_psg_mix(const char *psg_mix)
{
	if (option.psg_mix && strcmp(option.psg_mix, psg_mix) != 0)
		pr_warn("PSG filter \"%s\" replaces \"%s\"\n",
			psg_mix, option.psg_mix);

	option.psg_mix = psg_mix;
}

struct options *parse_options(int argc, char **argv)
{
	static const struct option options[] = {
		{ "help",                no_argument,       NULL, 0 },
		{ "version",             no_argument,       NULL, 0 },
		{ "verbose",             no_argument,       NULL, 0 },
		{ "quiet",               no_argument,       NULL, 0 },

		{ "info",                no_argument,       NULL, 0 },
		{ "output",              required_argument, NULL, 0 },

		{ "start",               required_argument, NULL, 0 },
		{ "stop",                required_argument, NULL, 0 },
		{ "length",              required_argument, NULL, 0 },

		{ "mode",                required_argument, NULL, 0 },

		{ "track",               required_argument, NULL, 0 },
		{ "frequency",           required_argument, NULL, 0 },

		{ "psg-mix",             required_argument, NULL, 0 },
		{ "psg-balance",         required_argument, NULL, 0 },
		{ "psg-volume",          required_argument, NULL, 0 },

		{ "disassemble",         no_argument,       NULL, 0 },
		{ "disassemble-header",  no_argument,       NULL, 0 },
		{ "disassemble-address", no_argument,       NULL, 0 },
		{ "remake-header",       no_argument,       NULL, 0 },

		{ "trace",               required_argument, NULL, 0 },

		{ NULL, 0, NULL, 0 }
	};

#define OPT(option) (strcmp(options[index].name, (option)) == 0)

	argv[0] = (char *)progname;	/* For better getopt_long messages. */

	option.track = -1;
	option.frequency = 44100;
	option.psg_mix = NULL;

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
			else if (OPT("quiet"))
				option.verbose--;

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
			else if (OPT("psg-mix"))
				set_psg_mix(optarg);
			else if (OPT("psg-balance")) {
				option.psg_balance = psg_balance_option(optarg);
				set_psg_mix("balance");
			}
			else if (OPT("psg-volume")) {
				option.psg_volume = psg_volume_option(optarg);
				set_psg_mix("volume");
			}

			else if (OPT("trace"))
				option.trace = trace_option(optarg);
			else if (OPT("disassemble"))
				option.disassemble = DISASSEMBLE_TYPE_ALL;
			else if (OPT("disassemble-header"))
				option.disassemble = DISASSEMBLE_TYPE_HEADER;
			else if (OPT("disassemble-address"))
				option.disassemble_address = true;
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
	if (!option.psg_mix)
		set_psg_mix("empiric");

	if (optind == argc)
		pr_fatal_error("missing input SNDH file\n");
	if (optind + 1 < argc)
		pr_fatal_error("%s: too many input SNDH files\n",
			argv[optind + 1]);

	option.input = argv[optind];

	return &option;
}
