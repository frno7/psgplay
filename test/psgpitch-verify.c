// SPDX-License-Identifier: GPL-2.0

#include <math.h>

#include "toslibc/asm/machine.h"

#include "test/report.h"
#include "test/verify.h"
#include "test/psgpitch.h"

test_value_names(int, tune_value_names);

static int tone_period(const struct options *options)
{
	return (int)round((double)ATARI_STE_EXT_OSC /
		(ATARI_STE_SND_PSG_CLK_DIV * 16.0 * test_value(options)));
}

static double tone_frequency(const struct options *options)
{
	return (double)ATARI_STE_EXT_OSC /
		(ATARI_STE_SND_PSG_CLK_DIV * 16.0 * tone_period(options));
}

void report(struct strbuf *sb, const struct audio *audio,
	const struct options *options)
{
	const struct test_wave_deviation wave_deviation =
		test_wave_deviation(audio);

	report_input(sb, audio, test_name(options), options);

	sbprintf(sb,
		"tone clock %d / %d / 16 Hz\n"
		"tone period %d cycles\n",
		ATARI_STE_EXT_OSC, ATARI_STE_SND_PSG_CLK_DIV,
		tone_period(options));

	report_wave_estimate(sb, audio->format, wave_deviation,
		tone_frequency(options));
}

const char *verify(const struct audio *audio, const struct options *options)
{
	const struct test_wave_deviation wave_deviation =
		test_wave_deviation(audio);
	const struct test_wave_error error = test_wave_error(
		audio->format, wave_deviation, tone_frequency(options));

	verify_assert (audio_duration(audio->format) >= 60.0)
		return "sample duration";

	verify_assert (wave_deviation.deviation.maximum <= 1.8)
		return "wave deviation max";

	verify_assert (error.relative_frequency <=
			audio_relative_tolerance(audio->format))
		return "wave error relative frequency";

	return NULL;
}

const char *flags(const struct options *options)
{
	return "--length 63";
}
