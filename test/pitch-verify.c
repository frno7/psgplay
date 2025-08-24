// SPDX-License-Identifier: GPL-2.0

#include <math.h>

#include "toslibc/asm/machine.h"

#include "test/report.h"
#include "test/pitch.h"

test_value_names(int, tune_value_names);

static int tone_period(const struct options *options)
{
	return (int)round(ATARI_STE_PSG_CLK / (16.0 * test_value(options)));
}

static double tone_frequency(const struct options *options)
{
	return ATARI_STE_PSG_CLK / (16.0 * tone_period(options));
}

void report(struct strbuf *sb, const struct audio *audio,
	const struct options *options)
{
	report_square_wave_estimate(sb, audio, test_name(options), options);

	sbprintf(sb,
		"tone period %d cycles\n"
		"tone frequency %f Hz\n",
		tone_period(options),
		tone_frequency(options));
}
