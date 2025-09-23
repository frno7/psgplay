// SPDX-License-Identifier: GPL-2.0

#include "test/report.h"
#include "test/verify.h"
#include "test/sndhfrms.h"

test_value_frames_names(int, tune_value_frames_names);

void report(struct strbuf *sb, const struct audio *audio,
	const struct options *options)
{
	const struct test_wave_deviation wave_deviation =
		test_wave_deviation(audio);

	report_input(sb, audio, test_name(options), options);

	/* One interrupt is half a period, so multiply with 0.5 accordingly. */
	report_wave_estimate(sb, audio->format, wave_deviation,
		0.5 * 96);
}

const char *verify(const struct audio *audio, const struct options *options)
{
	/*
	 * 1 frame with a 200 Hz timer and 44100 kHz
	 * sampling rate is 220.5 samples.
	 */
	verify_assert (audio->format.sample_count == 221)
		return "sample duration";

	return NULL;
}
