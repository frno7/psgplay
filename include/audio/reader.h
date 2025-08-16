// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2025 Fredrik Noring
 */

#ifndef PSGPLAY_AUDIO_READER_H
#define PSGPLAY_AUDIO_READER_H

#include "audio/audio.h"

struct audio_reader {
	void *(*open)(const char *path);
	struct audio_format (*format)(void *arg);
	size_t (*sample)(struct audio_sample *samples, size_t count, void *arg);
	void (*close)(void *arg);
};

#endif /* PSGPLAY_AUDIO_READER_H */
