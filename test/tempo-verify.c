// SPDX-License-Identifier: GPL-2.0

#include "toslibc/asm/machine.h"

#include "atari/machine.h"

#include "test/report.h"
#include "test/verify.h"
#include "test/tempo.h"

test_value_names(struct timer_preset, tune_value_names);

static double timer_frequency(const struct options *options)
{
	const struct timer_preset preset = test_value(options);

	return (double)ATARI_MFP_XTAL / (preset.divisor * preset.count);
}

void report(struct strbuf *sb, const struct audio *audio,
	const struct options *options)
{
	const struct timer_preset preset = test_value(options);
	const struct test_wave_deviation wave_deviation =
		test_wave_deviation(audio);

	report_input(sb, audio, test_name(options), options);

	sbprintf(sb,
		"timer clock %d Hz\n"
		"timer divisor %d cycles\n"
		"timer count %d cycles\n"
		"timer frequency %f Hz\n",
		ATARI_MFP_XTAL,
		preset.divisor,
		preset.count,
		timer_frequency(options));

	/* One interrupt is half a period, so multiply with 0.5 accordingly. */
	report_wave_estimate(sb, audio->format, wave_deviation,
		0.5 * timer_frequency(options));
}

const char *verify(const struct audio *audio, const struct options *options)
{
	const struct test_wave_deviation wave_deviation =
		test_wave_deviation(audio);

	/* One interrupt is half a period, so multiply with 0.5 accordingly. */
	const struct test_wave_error error = test_wave_error(
		audio->format, wave_deviation,
		0.5 * timer_frequency(options));

	verify_assert (audio_duration(audio->format) >= 60.0)
		return "sample duration";

	verify_assert (wave_deviation.deviation.maximum <= 5.0)
		return "wave deviation max";

	verify_assert (error.relative_frequency <=
			audio_relative_tolerance(audio->format))
		return "wave error relative frequency";

	return NULL;
}
