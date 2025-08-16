// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2025 Fredrik Noring
 */

#include <stdlib.h>

#include "system/unix/memory.h"

#include "audio/audio.h"

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
