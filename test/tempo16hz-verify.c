// SPDX-License-Identifier: GPL-2.0

#include "test/tempo16hz.h"
#include "test/report.h"

void report(struct strbuf *sb, const struct audio *audio,
	const struct options *options)
{
	report_square_wave_estimate(sb, audio, tune_name(options), options);
}
