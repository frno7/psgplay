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

#endif /* PSGPLAY_AUDIO_H */
