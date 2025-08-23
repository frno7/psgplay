// SPDX-License-Identifier: GPL-2.0

#include "test/report.h"

#include "test/tempo16hz.h"

tune_names(int, tune_value_names);

void report(const struct audio *audio, const struct options *options)
{
	report_square_wave_estimate(audio, tune_name(options), options);
}
