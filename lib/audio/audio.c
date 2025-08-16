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
