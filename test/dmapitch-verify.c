// SPDX-License-Identifier: GPL-2.0

#include <math.h>

#include "toslibc/asm/machine.h"

#include "test/report.h"
#include "test/verify.h"
#include "test/dmapitch.h"

test_value_names(struct dma_preset, tune_value_names);

static double dma_sound_frequency(const struct options *options)
{
	const int d = 1 << (3 - test_value(options).rate);

	return ATARI_STE_EXT_OSC / (double)(ATARI_STE_SND_DMA_CLK_DIV * d);
}

static int dma_sample_period(const struct options *options)
{
	return 2 * test_value(options).halfperiod;
}

static double dma_sample_frequency(const struct options *options)
{
	return dma_sound_frequency(options) / dma_sample_period(options);
}

void report(struct strbuf *sb, const struct audio *audio,
	const struct options *options)
{
	const struct test_wave_deviation wave_deviation =
		test_wave_deviation(audio);

	report_input(sb, audio, test_name(options), options);

	sbprintf(sb,
		"dma sound frequency %f Hz\n"
		"dma sample period %d samples\n",
		dma_sound_frequency(options),
		dma_sample_period(options));

	report_wave_estimate(sb, audio->format, wave_deviation,
		dma_sample_frequency(options));
}

const char *verify(const struct audio *audio, const struct options *options)
{
	const struct test_wave_deviation wave_deviation =
		test_wave_deviation(audio);
	const struct test_wave_error error = test_wave_error(
		audio->format, wave_deviation, dma_sample_frequency(options));

	verify_assert (audio_duration(audio->format) >= 60.0)
		return "sample duration";

	verify_assert (wave_deviation.deviation.maximum <= 1.5)
		return "wave deviation max";

	verify_assert (error.relative_frequency <= 5e-5) // FIXME: Accuracy
		return "wave error relative frequency";

	return NULL;
}

const char *flags(const struct options *options)
{
	return "--length 63";
}
