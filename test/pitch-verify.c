// SPDX-License-Identifier: GPL-2.0

#include <math.h>

#include "toslibc/asm/machine.h"

#include "test/report.h"
#include "test/pitch.h"

test_value_names(int, tune_value_names);

void report(struct strbuf *sb, const struct audio *audio,
	const struct options *options)
{
	report_square_wave_estimate(sb, audio, test_name(options), options);

	const int tone_period = (int)round(ATARI_STE_PSG_CLK /
		(16.0 * test_value(options)));
	const double tone_frequency = ATARI_STE_PSG_CLK / (16.0 * tone_period);

	sbprintf(sb,
		"tone period %d cycles\n"
		"tone frequency %f Hz\n",
		tone_period,
		tone_frequency);
}
