// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>
#include <string.h>

#include "atari/machine.h"

#include "psgplay/assert.h"
#include "psgplay/compare.h"
#include "psgplay/file.h"
#include "psgplay/print.h"
#include "psgplay/replay.h"
#include "psgplay/output.h"
#include "psgplay/tool.h"
#include "psgplay/sndh-tag.h"

struct replay_state {
	ssize_t sample_start;
	ssize_t sample_stop;

	const struct output *output;
	void *output_arg;

	ssize_t sample_count;
};

static bool replay_sample(s16 left, s16 right, void *arg)
{
	struct replay_state *state = arg;

	if (state->sample_count < state->sample_start) {
		state->sample_count++;
		return true;
	}

	state->sample_count++;

	if (0 <= state->sample_stop &&
		 state->sample_stop <= state->sample_count)
		return false;

	return state->output->sample(left, right, state->output_arg);
}

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

static ssize_t parse_stop_auto(int frequency, struct file file)
{
	float duration;

	if (!sndh_tag_time(&duration, file))
		return OPTION_TIME_UNDEFINED;

	return duration > 0 ?  roundf(duration * frequency) : OPTION_STOP_NEVER;
}

static ssize_t parse_stop(const char *s, int frequency, struct file file)
{
	return !s || strcmp(s, "auto") == 0 ? parse_stop_auto(frequency, file) :
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

void replay(const struct options *options, struct file file,
	const struct output *output, const struct machine *machine)
{
	const ssize_t start = parse_start(options->start, options->frequency);
	const ssize_t stop = parse_stop(
		options->stop, options->frequency, file);
	const ssize_t length = parse_length(
		options->length, options->frequency, start);

	struct replay_state state = {
		.sample_start = start,
		.sample_stop = stop_or_length(stop, length),

		.output = output,
		.output_arg = output->open(options->output,
			options->track, options->frequency),
	};

	machine->init(file.data, file.size, options->track,
		options->frequency, replay_sample, &state);

	for (;;)
		if (!machine->run())
			break;

	output->close(state.output_arg);
}
