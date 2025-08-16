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

struct audio *audio_alloc(struct audio_format format);

void audio_free(struct audio *audio);

struct audio *audio_read_wave(const char *path);

struct audio *audio_range(const struct audio *audio,
	size_t offset, size_t length);

#endif /* PSGPLAY_AUDIO_H */
