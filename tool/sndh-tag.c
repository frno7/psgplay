// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "psgplay/file.h"
#include "psgplay/print.h"
#include "psgplay/sndh.h"
#include "psgplay/sndh-print.h"
#include "psgplay/string.h"

struct tag_time_state {
	int t;
	int track;

	float *duration;
};

static bool subtune_count(const char *name, const char *value, void *arg)
{
	int *track_count = arg;

	if (strcmp(name, "##") != 0)
		return true;

	*track_count = atoi(value);

	return false;
}

bool sndh_tag_subtune_count(int *track_count, struct file file)
{
	return !sndh_tags(file, NULL, subtune_count, track_count);
}

static bool tag_default_subtune(const char *name, const char *value, void *arg)
{
	int *track = arg;

	if (strcmp(name, "!#") != 0)
		return true;

	*track = atoi(value);

	return false;
}

bool sndh_tag_default_subtune(int *track, struct file file)
{
	return !sndh_tags(file, NULL, tag_default_subtune, track);
}

static bool tag_time(const char *name, const char *value, void *arg)
{
	struct tag_time_state *state = arg;

	if (strcmp(name, "TIME") != 0)
		return true;

	if (++state->t != state->track)
		return true;

	*state->duration = atof(value);

	return false;
}

bool sndh_tag_time(float *duration, int track, struct file file)
{
	struct tag_time_state state = { .track = track, .duration = duration };

	return !sndh_tags(file, NULL, tag_time, &state);
}
