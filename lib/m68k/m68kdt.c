// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "internal/assert.h"
#include "internal/build-assert.h"
#include "internal/compare.h"
#include "internal/print.h"
#include "internal/types.h"

#include "m68k/m68kda.h"
#include "m68k/m68kdg.h"

#include "psgplay/version.h"

const char *progname = "m68kdt";

static struct options {
	int verbose;
} option;

static void help(FILE *file)
{
	fprintf(file,
"Usage: %s [options]...\n"
"\n"
"General options:\n"
"\n"
"    -h, --help             display this help and exit\n"
"    --version              display version and exit\n"
"    -v, --verbose          increase verbosity\n"
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

static void parse_options(int argc, char **argv)
{
	static const struct option options[] = {
		{ "help",    no_argument, NULL, 0 },
		{ "version", no_argument, NULL, 0 },
		{ "verbose", no_argument, NULL, 0 },

		{ NULL, 0, NULL, 0 }
	};

#define OPT(option) (strcmp(options[index].name, (option)) == 0)

	argv[0] = progname;	/* For better getopt_long error messages. */

	for (;;) {
		int index = 0;

		switch (getopt_long(argc, argv,
			"hv", options, &index)) {
		case -1:
			if (optind != argc)
				help_exit(EXIT_FAILURE);
			return;

		case 0:
			if (OPT("help"))
				goto opt_h;
			else if (OPT("version"))
				version_exit();
			else if (OPT("verbose"))
				goto opt_v;

		case 'h':
opt_h:			help_exit(EXIT_SUCCESS);

		case 'v':
opt_v:			option.verbose++;
			break;

		case '?':
			exit(EXIT_FAILURE);
		}
	}

#undef OPT
}

#define ASSERT(x)							\
	do {								\
		if (option.verbose > 1)					\
			puts("assert:" XSTR(__LINE__) ": " STR(x));	\
		BUG_ON(!(x));						\
	} while (0)

static void test_insn_find(void)
{
	static const struct m68kda_spec opcodes[] =
	{
		M68KDG_INSTRUCTIONS(M68KDG_INSTRUCTION_SPEC)
	};

	ASSERT(ARRAY_SIZE(opcodes) == 1533);

	for (uint32_t m = 0x0000; m < 0x10000; m++) {
		const struct m68kda_spec *spec0 = NULL;

		for (int i = 0; !spec0 && i < ARRAY_SIZE(opcodes); i++)
			if ((m & opcodes[i].mask) == opcodes[i].code)
				spec0 = &opcodes[i];

		const struct m68kda_spec *spec1 = m68kda_insn_find(
			(union m68kda_insn) { .word = m });

		if (spec0) {
			ASSERT(spec1);

			ASSERT(strcmp(spec0->mnemonic, spec1->mnemonic) == 0);

			ASSERT(spec0->code       == spec1->code);
			ASSERT(spec0->mask       == spec1->mask);

			ASSERT(spec0->op0.size   == spec1->op0.size);
			ASSERT(spec0->op0.opcp.c == spec1->op0.opcp.c);
			ASSERT(spec0->op0.opcp.p == spec1->op0.opcp.p);

			ASSERT(spec0->op1.size   == spec1->op1.size);
			ASSERT(spec0->op1.opcp.c == spec1->op1.opcp.c);
			ASSERT(spec0->op1.opcp.p == spec1->op1.opcp.p);
		} else
			ASSERT(!spec1);
	}
}

#define TEST(f_) { .name = #f_, .f = test_ ## f_ }

static const struct {
	const char *name;
	void (*f)(void);
} tests[] = {
	TEST(insn_find),
};

int main(int argc, char *argv[])
{
	parse_options(argc, argv);

	for (size_t i = 0; i < ARRAY_SIZE(tests); i++) {
		if (option.verbose)
			printf("test %s\n", tests[i].name);

		tests[i].f();
	}

	return EXIT_SUCCESS;
}
