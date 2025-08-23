// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2025 Fredrik Noring
 */

#ifndef PSGPLAY_TEST_REPORT_H
#define PSGPLAY_TEST_REPORT_H

#include "audio/audio.h"

#include "test/option.h"

#define TUNE_DEFINE_TITLE(value, title)	title,

#define tune_names(type, names)						\
	const char *tune_names_[] = {					\
		names(TUNE_DEFINE_TITLE)				\
	}

static inline const char *tune_name(int tune)
{
	extern const char *tune_names_[];

	return tune_names_[tune - 1];
}

void report(int tune, const struct audio *audio, const struct options *options);

void report_square_wave_estimate(int tune, const char *name,
	const struct audio *audio, const struct options *options);

#endif /* PSGPLAY_TEST_REPORT_H */
