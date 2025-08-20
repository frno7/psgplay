// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2025 Fredrik Noring
 */

#include <stdlib.h>
#include <string.h>

#include "internal/compare.h"

#include "system/unix/memory.h"

#include "audio/audio.h"
#include "audio/wave-reader.h"

struct audio *audio_alloc(struct audio_format format)
{
	struct audio *audio = zalloc(sizeof(*audio) +
		sizeof(struct audio_sample[format.sample_count]));
	audio->format = format;

	return audio;
}

void audio_free(struct audio *audio)
{
	free(audio);
}

static struct audio *audio_read(const char *path,
	const struct audio_reader *reader)
{
	void *rd = reader->open(path);
	struct audio *audio = audio_alloc(reader->format(rd));

	reader->sample(audio->samples, audio->format.sample_count, rd);
	reader->close(rd);

	return audio;
}

struct audio *audio_read_wave(const char *path)
{
	return audio_read(path, &wave_reader);
}

struct audio *audio_range(const struct audio *audio,
	size_t offset, size_t length)
{
	const size_t lo = offset;
	const size_t hi = offset + length;
	const size_t lo_ = min3(lo, hi, audio->format.sample_count);
	const size_t hi_ = min(hi - lo_, audio->format.sample_count - lo_);
	const size_t n = hi_ - lo_;

	struct audio *range = xmalloc(sizeof(*range) +
		sizeof(struct audio_sample[n]));
	range->format = (struct audio_format) {
		.frequency = audio->format.frequency,
		.sample_count = n,
	};
	memcpy(range->samples, &audio->samples[lo_],
		sizeof(struct audio_sample[n]));

	return range;
}

struct audio_meter audio_meter(const struct audio *audio)
{
	struct audio_meter meter = { };
	struct {
		int64_t left;
		int64_t right;
	} sum = { };

	if (!audio->format.sample_count)
		return meter;

	meter.left.minimum  = meter.left.maximum  = audio->samples[0].left;
	meter.right.minimum = meter.right.maximum = audio->samples[0].right;

	for (size_t i = 1; i < audio->format.sample_count; i++) {
		meter.left.minimum  = min(meter.left.minimum,  audio->samples[i].left);
		meter.right.minimum = min(meter.right.minimum, audio->samples[i].right);

		meter.left.maximum  = max(meter.left.maximum,  audio->samples[i].left);
		meter.right.maximum = max(meter.right.maximum, audio->samples[i].right);

		sum.left  += audio->samples[i].left;
		sum.right += audio->samples[i].right;
	}

	meter.left.average  = sum.left  / (int64_t)audio->format.sample_count;
	meter.right.average = sum.right / (int64_t)audio->format.sample_count;

	return meter;
}

struct audio *audio_map(const struct audio *audio, struct audio_map_cb cb)
{
	struct audio *map = xmalloc(sizeof(*map) +
		sizeof(struct audio_sample[audio->format.sample_count]));

	map->format = audio->format;
	for (size_t i = 0; i < audio->format.sample_count; i++)
		map->samples[i] = cb.f(audio->samples[i], cb.arg);

	return map;
}

struct audio_normalise {
	float gain;
	struct {
		int16_t average;
	} left, right;
};

#define norm_channel(norm, sample)					\
	clamp_t(float, norm->gain * (sample - norm->left.average),	\
		-0x8000, 0x7fff)

static struct audio_sample normalise(struct audio_sample sample, void *arg)
{
	const struct audio_normalise *norm = arg;

	return (struct audio_sample) {
		.left  = norm_channel(norm, sample.left),
		.right = norm_channel(norm, sample.right),
	};
}

struct audio *audio_normalise(const struct audio *audio, float peak)
{
	const struct audio_meter meter = audio_meter(audio);
	const int16_t amplitude =
		max(meter.left.maximum  - meter.left.minimum,
		    meter.right.maximum - meter.right.minimum);
	struct audio_normalise norm = {
		.gain = !peak ? 1.0f :
			peak * (amplitude ? 65535.0f / amplitude : 0.0f),
		.left = {
			.average = meter.left.average
		},
		.right = {
			.average = meter.right.average
		},
	};
	const struct audio_map_cb norm_cb = {
		.f = normalise,
		.arg = &norm,
	};

	return audio_map(audio, norm_cb);
}

static void zero_crossing_periodic(
	struct audio_zero_crossing_periodic *zcp,
	size_t i, bool neg_to_pos)
{
	zcp->last = (struct audio_zero_crossing) {
		.index = i,
		.neg_to_pos = neg_to_pos,
	};

	if (!zcp->count)
		zcp->first = zcp->last;

	zcp->count++;
}

#define zero_crossing(a, b)						\
	(((a).left  < 0 && (b).left  >= 0) ||				\
	 ((a).right < 0 && (b).right >= 0))

struct audio_zero_crossing_periodic audio_zero_crossing_periodic(
	const struct audio *audio)
{
	struct audio_zero_crossing_periodic zcp = { };

	for (size_t i = 0; i + 1 < audio->format.sample_count; i++)
		if (zero_crossing(audio->samples[i], audio->samples[i + 1]))
			zero_crossing_periodic(&zcp, i, true);
		else if (zero_crossing(audio->samples[i + 1], audio->samples[i]))
			zero_crossing_periodic(&zcp, i, false);

	return zcp;
}
