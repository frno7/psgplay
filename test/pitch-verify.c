// SPDX-License-Identifier: GPL-2.0

#include "test/report.h"

#include "test/pitch.h"

tune_names(int, tune_value_names);

void report(int tune, const struct audio *audio, const struct options *options)
{
	report_square_wave_estimate(tune, tune_name(tune), audio, options);
}
