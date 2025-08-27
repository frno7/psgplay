// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2025 Fredrik Noring
 */

#ifndef PSGPLAY_AUDIO_H
#define PSGPLAY_AUDIO_H

#include "internal/types.h"

struct audio_format {
	const char *path;
	int frequency;
	size_t sample_count;
};

struct audio_sample {
	int16_t left;
	int16_t right;
};

struct audio {
	struct audio_format format;
	struct audio_sample samples[];
};

struct audio_meter {
	struct audio_meter_channel {
		int16_t minimum;
		int16_t maximum;
		int16_t average;
	} left, right;
};

struct audio_map_cb {
	struct audio_sample (*f)(struct audio_sample sample, void *arg);
	void *arg;
};

struct audio_zero_crossing {
	size_t index;
	bool neg_to_pos;
};

struct audio_zero_crossing_cb {
	bool (*f)(size_t index, bool neg_to_pos, void *arg);
	void *arg;
};

struct audio_zero_crossing_periodic {
	size_t count;
	struct audio_zero_crossing first;
	struct audio_zero_crossing last;
};

struct audio_zero_crossing_periodic_deviation {
	size_t count;
	double maximum;
};

struct audio_wave {
	double period;
	double phase;
};

struct audio *audio_alloc(struct audio_format format);

void audio_free(struct audio *audio);

struct audio *audio_read_wave(const char *path);

struct audio *audio_range(const struct audio *audio,
	size_t offset, size_t length);

struct audio_meter audio_meter(const struct audio *audio);

struct audio *audio_map(const struct audio *audio, struct audio_map_cb cb);

struct audio *audio_normalise(const struct audio *audio, float peak);

bool audio_zero_crossing(const struct audio *audio,
	struct audio_zero_crossing_cb cb);

struct audio_zero_crossing_periodic audio_zero_crossing_periodic(
	const struct audio *audio);

struct audio_zero_crossing_periodic_deviation
	audio_zero_crossing_periodic_deviation(const struct audio *audio,
		struct audio_wave wave);

struct audio_wave audio_wave_estimate(struct audio_zero_crossing_periodic zcp);

static inline double audio_frequency_from_period(double period,
	double sampling_frequency)
{
	return period ? sampling_frequency / period : 0.0;
}

static inline double audio_duration(const struct audio_format format)
{
	return format.sample_count / (double)format.frequency;
}

#endif /* PSGPLAY_AUDIO_H */
