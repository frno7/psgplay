// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#include <errno.h>
#include <fcntl.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "internal/assert.h"
#include "internal/compare.h"
#include "internal/print.h"

#include "psgplay/psgplay.h"
#include "psgplay/sndh.h"

#include "out/output.h"

#include "system/unix/file.h"
#include "system/unix/memory.h"
#include "system/unix/option.h"
#include "system/unix/command-mode.h"

struct replay_state {
	ssize_t sample_start;
	ssize_t sample_stop;

	const struct output *output;
	void *output_arg;

	ssize_t sample_count;
};

static ssize_t parse_time(const char *s, int frequency)
{
	float a, b;
	const int r = sscanf(s, "%f:%f", &a, &b);

	if (!r)
		pr_fatal_error("%s: malformed time '%s'\n", progname, s);

	return roundf((r == 2 ? 60.0f * a + b : a) * frequency);
}

static ssize_t parse_start(const char *s, int frequency)
{
	return !s ? 0 : parse_time(s, frequency);
}

static ssize_t parse_stop_auto(int track, int frequency, struct file file)
{
	float duration;

	if (!sndh_tag_subtune_time(&duration, track, file.data, file.size))
		return OPTION_TIME_UNDEFINED;

	return duration > 0 ?  roundf(duration * frequency) : OPTION_STOP_NEVER;
}

static ssize_t parse_stop(const char *s,
	int track, int frequency, struct file file)
{
	return !s                      ? OPTION_TIME_UNDEFINED :
	       strcmp(s, "auto") == 0  ? parse_stop_auto(track, frequency, file) :
	       strcmp(s, "never") == 0 ? OPTION_STOP_NEVER :
					 parse_time(s, frequency);
}

static ssize_t parse_length(const char *s, int frequency, ssize_t start)
{
	return !s ? OPTION_TIME_UNDEFINED : start + parse_time(s, frequency);
}

static ssize_t stop_or_length(ssize_t stop, ssize_t length)
{
	return   stop == OPTION_TIME_UNDEFINED ? length :
	       length == OPTION_TIME_UNDEFINED ? stop   :
		 stop == OPTION_STOP_NEVER     ? length : min(stop, length);
}

void command_replay(const struct options *options, struct file file,
	const struct output *output)
{
	const char *auto_stop = options->stop ? options->stop :
		!options->stop && !options->length ? "auto" : NULL;
	const ssize_t sample_start = parse_start(
		options->start, options->frequency);
	const ssize_t stop = parse_stop(
		auto_stop, options->track, options->frequency, file);
	const ssize_t length = parse_length(
		options->length, options->frequency, sample_start);
	const ssize_t sample_stop = stop_or_length(stop, length);
	void *output_arg = output->open(
		options->output, options->frequency, false);
	struct psgplay *pp = psgplay_init(file.data, file.size,
		options->track, options->frequency);
	ssize_t sample_count = 0;

	if (!pp)
		pr_fatal_error("%s: failed to init PSG play\n", progname);

	psgplay_digital_to_stereo_callback(pp, psg_mix_option(), NULL);

	for (;;) {
		struct psgplay_stereo buffer[256];

		const ssize_t r = psgplay_read_stereo(
			pp, buffer, ARRAY_SIZE(buffer));

		if (!r)
			break;

		for (size_t i = 0; i < r; i++) {
			if (sample_count < sample_start) {
				sample_count++;
				continue;
			}

			sample_count++;

			if (0 <= sample_stop &&
				 sample_stop <= sample_count)
				goto out;

			if (!output->sample(
					buffer[i].left,
					buffer[i].right, output_arg))
				goto out;
		}
	}
out:

	psgplay_free(pp);

	output->close(output_arg);
}
