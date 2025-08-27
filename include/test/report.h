// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2025 Fredrik Noring
 */

#ifndef PSGPLAY_TEST_REPORT_H
#define PSGPLAY_TEST_REPORT_H

#include "audio/audio.h"

#include "system/unix/string.h"

#include "test/option.h"

#define TEST_DEFINE_VALUE(value, name)	value,

#define test_values(type, names)					\
	const type test_values_[] = {					\
		names(TEST_DEFINE_VALUE)				\
	}

#define TEST_DEFINE_NAME(value, name)	name,

#define test_names(names)						\
	const char *test_names_[] = {					\
		names(TEST_DEFINE_NAME)					\
	}

#define test_value_names(type, names)					\
	test_values(type, names);					\
	test_names(names);						\
									\
	static inline type test_value(const struct options *options)	\
	{								\
		return test_values_[options->track - 1];		\
	}								\
									\
	static inline const char *test_name(const struct options *options) \
	{								\
		return test_names_[options->track - 1];			\
	}

struct test_wave_deviation {
	struct audio_wave wave;
	struct audio_zero_crossing_periodic_deviation deviation;
};

struct test_wave_error {
	double absolute_frequency;
	double relative_frequency;
};

void report(struct strbuf *sb, const struct audio *audio,
	const struct options *options);

void report_input(struct strbuf *sb, const struct audio *audio,
	const char *name, const struct options *options);

struct test_wave_deviation test_wave_deviation(const struct audio *audio);

struct test_wave_error test_wave_error(struct audio_format audio_format,
	struct test_wave_deviation wave_deviation, double reference_frequency);

void report_wave_estimate(struct strbuf *sb, struct audio_format audio_format,
	struct test_wave_deviation wave_deviation, double reference_frequency);

#endif /* PSGPLAY_TEST_REPORT_H */
