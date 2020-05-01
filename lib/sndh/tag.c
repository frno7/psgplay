// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#include <string.h>

#include "internal/macro.h"

#include "sndh/sndh.h"
#include "sndh/tag.h"

bool sndh_tag_subtune_count(int *subtune_count,
	const void *data, const size_t size)
{
	sndh_for_each_tag (data, size)
		if (strcmp(sndh_tag_name, "##") == 0) {
			*subtune_count = sndh_tag_integer;

			return true;
		}

	return false;
}

bool sndh_tag_default_subtune(int *default_subtune,
	const void *data, const size_t size)
{
	sndh_for_each_tag (data, size)
		if (strcmp(sndh_tag_name, "!#") == 0) {
			*default_subtune = sndh_tag_integer;

			return true;
		}

	return false;
}

bool sndh_tag_subtune_time(float *duration, int subtune,
	const void *data, const size_t size)
{
	int st = 0;

	sndh_for_each_tag (data, size)
		if (strcmp(sndh_tag_name, "TIME") == 0 && ++st == subtune) {
			*duration = sndh_tag_integer;

			return true;
		}

	return false;
}

bool sndh_tag_timer(struct sndh_timer *timer,
	const void *data, const size_t size)
{
	static const struct {
		const char *name;
		enum sndh_timer_type type;
	} timers[] = {
		{ "TA", SNDH_TIMER_A },
		{ "TB", SNDH_TIMER_B },
		{ "TC", SNDH_TIMER_C },
		{ "TD", SNDH_TIMER_D },
		{ "!V", SNDH_TIMER_V },
	};

	sndh_for_each_tag (data, size)
		for (size_t i = 0; i < ARRAY_SIZE(timers); i++)
			if (strcmp(sndh_tag_name, timers[i].name) == 0) {
				*timer = (struct sndh_timer) {
					.type = timers[i].type,
					.frequency = sndh_tag_integer
				};

				return true;
			}

	return false;
}
