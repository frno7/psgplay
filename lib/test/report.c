// SPDX-License-Identifier: GPL-2.0

#include <stdio.h>
#include <string.h>

#include "internal/print.h"

#include "audio/audio.h"
#include "system/unix/file.h"

#include "test/option.h"

void report_square_wave_estimate(const struct audio *audio,
	const char *name, const struct options *options)
{
	struct audio *norm = audio_normalise(audio, 0.8f);
	const struct audio_zero_crossing_periodic zcp =
		audio_zero_crossing_periodic(norm);
	const struct audio_wave wave = audio_wave_estimate(zcp);
	char report[1024];

	snprintf(report, sizeof(report),
		"path %s\n"
		"index %d\n"
		"name %s\n"
		"sample count %zu samples\n"
		"sample duration %.1f s\n"
		"sample frequency %d Hz\n"
		"square wave period %f samples\n"
		"square wave frequency %f Hz\n"
		"square wave phase %f samples\n"
		"square wave deviation max %f samples\n",
		options->input,
		options->track,
		name,
		audio->format.sample_count,
		audio->format.sample_count / (double)audio->format.frequency,
		audio->format.frequency,
		wave.period,
		wave.period ? audio->format.frequency / wave.period : 0.0,
		wave.phase,
		audio_zero_crossing_periodic_deviation(norm, wave).maximum);

	if (!file_write(options->output, report, strlen(report)))
		pr_fatal_errno(options->output);

	audio_free(norm);
}
