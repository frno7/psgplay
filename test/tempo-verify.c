// SPDX-License-Identifier: GPL-2.0

#include "test/tempo.h"
#include "test/report.h"

test_value_names(int, tune_value_names);

void report(struct strbuf *sb, const struct audio *audio,
	const struct options *options)
{
	const struct test_wave_deviation deviation =
		test_wave_deviation(audio);

	report_input(sb, audio, test_name(options), options);
	report_wave_estimate(sb, audio->format, deviation);
}
