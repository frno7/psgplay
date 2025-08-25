// SPDX-License-Identifier: GPL-2.0

#include <math.h>
#include <stdio.h>
#include <string.h>

#include "internal/print.h"

#include "audio/audio.h"
#include "system/unix/file.h"

#include "test/option.h"
#include "test/report.h"

void report_input(struct strbuf *sb, const struct audio *audio,
	const char *name, const struct options *options)
{
	sbprintf(sb,
		"path %s\n"
		"name %s\n"
		"index %d\n"
		"title %s\n"
		"sample count %zu samples\n"
		"sample duration %.1f s\n"
		"sample frequency %d Hz\n",
		options->input,
		options->name,
		options->track,
		name,
		audio->format.sample_count,
		audio_duration(audio->format),
		audio->format.frequency);
}

struct test_wave_deviation test_wave_deviation(const struct audio *audio)
{
	struct audio *norm = audio_normalise(audio, 0.8f);

	const struct audio_zero_crossing_periodic zcp =
		audio_zero_crossing_periodic(norm);
	const struct audio_wave wave = audio_wave_estimate(zcp);
	const struct audio_zero_crossing_periodic_deviation deviation =
		audio_zero_crossing_periodic_deviation(norm, wave);

	audio_free(norm);

	return (struct test_wave_deviation) {
		.wave = wave,
		.deviation = deviation,
	};
}

struct test_wave_error test_wave_error(struct audio_format audio_format,
	struct test_wave_deviation wave_deviation, double reference_frequency)
{
	const double absolute_frequency = audio_frequency_from_period(
		wave_deviation.wave.period, audio_format.frequency) -
		reference_frequency;

	return (struct test_wave_error) {
		.absolute_frequency = absolute_frequency,
		.relative_frequency =
			fabs(absolute_frequency) / reference_frequency,
	};
}

void report_wave_estimate(struct strbuf *sb, struct audio_format audio_format,
	struct test_wave_deviation wave_deviation)
{
	sbprintf(sb,
		"wave period %f samples\n"
		"wave frequency %f Hz\n"
		"wave phase %f samples\n"
		"wave phase deviation max %f samples\n",
		wave_deviation.wave.period,
		audio_frequency_from_period(
			wave_deviation.wave.period,
			audio_format.frequency),
		wave_deviation.wave.phase,
		wave_deviation.deviation.maximum);
}
