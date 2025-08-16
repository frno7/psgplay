// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2025 Fredrik Noring
 */

#include <stdlib.h>

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
