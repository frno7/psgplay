// SPDX-License-Identifier: GPL-2.0

#include "internal/compare.h"

#include "test/report.h"
#include "test/verify.h"
#include "test/maxamp.h"

test_value_names(int, tune_value_names);

struct minmax {
	int16_t minimum;
	int16_t maximum;
};

static struct minmax minmax(const struct audio *audio)
{
	struct minmax mm = { };

	if (!audio->format.sample_count)
		return mm;

	mm.minimum = min(audio->samples[0].left, audio->samples[0].right);
	mm.maximum = max(audio->samples[0].left, audio->samples[0].right);

	for (size_t i = 1; i < audio->format.sample_count; i++) {
		mm.minimum = min3(mm.minimum, audio->samples[i].left,
					      audio->samples[i].right);
		mm.maximum = max3(mm.maximum, audio->samples[i].left,
					      audio->samples[i].right);
	}

	return mm;
}

void report(struct strbuf *sb, const struct audio *audio,
	const struct options *options)
{
	const struct minmax mm = minmax(audio);

	report_input(sb, audio, test_name(options), options);

	sbprintf(sb,
		"wave minimum %d\n"
		"wave maximum %d\n",
		mm.minimum,
		mm.maximum);
}

const char *verify(const struct audio *audio, const struct options *options)
{
	const struct minmax mm = minmax(audio);

	verify_assert (audio_duration(audio->format) >= 1.0)
		return "sample duration";

	verify_assert (mm.minimum < -32000 &&	/* Verify near numerical min */
		       mm.minimum > -32760)	/* Verify margin to clipping */
		return "sample minimum";

	verify_assert (mm.maximum > 32000 &&	/* Verify near numerical max */
		       mm.maximum < 32760)	/* Verify margin to clipping */
		return "sample maximum";

	return NULL;
}

const char *flags(const struct options *options)
{
	return "--length 4";
}
