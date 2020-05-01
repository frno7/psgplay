// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "atari/machine.h"

#include "psgplay/build-bug.h"
#include "psgplay/compare.h"
#include "psgplay/diagnostic.h"
#include "psgplay/info.h"
#include "psgplay/option.h"
#include "psgplay/print.h"
#include "psgplay/replay.h"
#include "psgplay/types.h"

#include "sndh/read.h"
#include "sndh/sndh.h"
#include "sndh/tag.h"

#include "out/alsa.h"
#include "out/wave.h"

#include "system/unix/file.h"

char progname[] = "psgplay";

static void NORETURN info_exit(struct file file)
{
	sndh_print(file);

	exit(EXIT_SUCCESS);
}

static int default_subtune(struct file file)
{
	int track;

	if (!sndh_tag_default_subtune(&track, file.data, file.size))
		track = 1;

	return track;
}

static void select_subtune(int *track, struct file file)
{
	int track_count;

	if (*track < 1)
		*track = default_subtune(file);

	if (sndh_tag_subtune_count(&track_count, file.data, file.size))
		if (*track > track_count)
			pr_fatal_error("%s: track %d out of range 1 to %d\n",
				file.path, *track, track_count);
}

static const struct output *select_output(const struct options *options)
{
	const struct output *output = options->output ? &wave_output : NULL;

#ifdef HAVE_ALSA
	if (!output)
		output = &alsa_output;
#endif /* HAVE_ALSA */

	if (!output)
		pr_fatal_error("missing output file\n");

	return output;
}

int main(int argc, char *argv[])
{
	struct options *options = parse_options(argc, argv);

	struct file file = sndh_read_file(options->input);
	if (!file_valid(file))
		pr_fatal_errno(options->input);

	sndh_diagnostic(file);

	if (options->info)
		info_exit(file);

	select_subtune(&options->track, file);

	replay(options, file, select_output(options), &atari_st);

	file_free(file);

	return EXIT_SUCCESS;
}
