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

	if (!preset.ctrl && preset.frequency == 50)
		return CPU_FREQUENCY / (double)ATARI_STE_CYCLES_PER_VBL_PAL;
	if (!preset.ctrl)
		return preset.frequency;

	return (double)ATARI_MFP_XTAL / (preset.divisor * preset.count);
}

#define INIT								\
	/*								\
	 * One interrupt is half a period, so				\
	 * multiply with 0.5 and 2.0 accordingly.			\
	 */								\
	const struct test_wave_deviation wave_deviation =		\
		test_wave_deviation(audio);				\
	const struct test_wave_error error = test_wave_error(		\
		audio->format, wave_deviation,				\
		0.5 * timer_frequency(options))

void report(struct strbuf *sb, const struct audio *audio,
	const struct options *options)
{
	INIT;

	const struct timer_preset preset = test_value(options);
	const double interrupt_frequency = 2.0 *
		audio_frequency_from_period(
			wave_deviation.wave.period,
			audio->format.frequency);

	report_input(sb, audio, test_name(options), options);

	if (preset.ctrl)
		sbprintf(sb,
			"timer clock %d Hz\n"
			"timer divisor %d cycles\n"
			"timer count %d cycles\n",
			ATARI_MFP_XTAL,
			preset.divisor,
			preset.count);
	else if (preset.frequency == 50)
		sbprintf(sb,
			"cpu clock %d Hz\n"
			"vbl count %d cycles\n",
			CPU_FREQUENCY,
			ATARI_STE_CYCLES_PER_VBL_PAL);

	sbprintf(sb, "timer frequency %f Hz\n", timer_frequency(options));

	report_wave_estimate(sb, audio->format, wave_deviation);

	sbprintf(sb,
		"interrupt frequency %f Hz\n"
		"interrupt error absolute frequency %f Hz\n"
		"interrupt error relative frequency %.2e\n",
		interrupt_frequency,
		2.0 * error.absolute_frequency,
		error.relative_frequency);
}

char *verify(const struct audio *audio, const struct options *options)
{
	INIT;

	verify_assert (audio_duration(audio->format) >= 60.0)
		return "sample duration";

	verify_assert (wave_deviation.deviation.maximum <= 5.0)
		return "wave deviation max";

	verify_assert (error.relative_frequency <= 9e-7)
		return "wave error relative frequency";

	return NULL;
}
