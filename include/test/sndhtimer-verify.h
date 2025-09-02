// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2025 Fredrik Noring
 */

#ifndef PSGPLAY_TEST_TIMER_SNDH_VERIFY_H
#define PSGPLAY_TEST_TIMER_SNDH_VERIFY_H

test_value_names(double, tune_value_names);

#define TIMER_SNDH_VERIFY_INIT						\
	const double timer_frequency = test_value(options);		\
	const struct test_wave_deviation wave_deviation =		\
		test_wave_deviation(audio)

void report(struct strbuf *sb, const struct audio *audio,
	const struct options *options)
{
	TIMER_SNDH_VERIFY_INIT;

	report_input(sb, audio, test_name(options), options);

	sbprintf(sb, "timer frequency %f Hz\n", timer_frequency);

	/* One interrupt is half a period, so multiply with 0.5 accordingly. */
	report_wave_estimate(sb, audio->format, wave_deviation,
		0.5 * timer_frequency);
}

char *verify(const struct audio *audio, const struct options *options)
{
	TIMER_SNDH_VERIFY_INIT;

	/* One interrupt is half a period, so multiply with 0.5 accordingly. */
	const struct test_wave_error error = test_wave_error(
		audio->format, wave_deviation,
		0.5 * timer_frequency);

	verify_assert (audio_duration(audio->format) >= 60.0)
		return "sample duration";

	verify_assert (wave_deviation.deviation.maximum <= 5.0)
		return "wave deviation max";

	verify_assert (error.relative_frequency <=
			audio_relative_tolerance(audio->format))
		return "wave error relative frequency";

	return NULL;
}

#endif /* PSGPLAY_TEST_TIMER_SNDH_VERIFY_H */
