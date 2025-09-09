// SPDX-License-Identifier: GPL-2.0

#include "toslibc/asm/machine.h"

#include "atari/machine.h"

#include "test/report.h"
#include "test/verify.h"
#include "test/dmasint.h"

test_value_time_names(struct dma_preset, tune_value_time_names);

static double dma_sound_frequency(const struct options *options)
{
	const int d = 1 << (3 - test_value(options).rate);

	return ATARI_STE_EXT_OSC / (double)(ATARI_STE_SND_DMA_CLK_DIV * d);
}

static int dma_sample_period(const struct options *options)
{
	return 2 * test_value(options).halfperiod;
}

struct zero_crossing {
	const struct audio *audio;
	const double period;

	size_t first;
	size_t last;
	size_t count;

	size_t x, y, z;

	const char *error;
};

static int diff(size_t i, size_t m, const struct audio *audio)
{
	return m <= i && i + m < audio->format.sample_count ?
		audio->samples[i - m].left + audio->samples[i - m].right -
		audio->samples[i + m].left - audio->samples[i + m].right : 0;
}

static bool zero_crossing(size_t i, struct audio_sample a,
	struct audio_sample b, void *arg)
{
	struct zero_crossing *zc = arg;

	const int d = diff(i, 10, zc->audio);

	if (d < 8000)
		return true;
	else if (d < 16000) zc->x++;
	else if (d < 32000) zc->y++;
	else		    zc->z++;

	if (zc->count) {
		const size_t p = i - zc->last;

		if (p < zc->period - 2 ||
		    p > zc->period + 2)
			zc->error = "period malfunction";
	} else
		zc->first = i;
	zc->last = i;
	zc->count++;

	return true;
}

static struct zero_crossing measure(const struct audio *audio,
	const struct options *options)
{
	struct audio *norm = audio_normalise(audio, 0.8f);
	struct zero_crossing zc = {
		.audio = audio,
		.period = audio->format.frequency  *
			dma_sample_period(options) /
			dma_sound_frequency(options),
	};

	audio_zero_crossing(norm, (struct audio_zero_crossing_cb) {
			.f = zero_crossing,
			.arg = &zc,
		});

	audio_free(norm);

	return zc;
}

void report(struct strbuf *sb, const struct audio *audio,
	const struct options *options)
{
	const struct zero_crossing zc = measure(audio, options);

	report_input(sb, audio, test_name(options), options);

	sbprintf(sb,
		"dma sound frequency %f Hz\n"
		"dma sample period %d samples\n"
		"dma sample crossings %zu %zu %zu\n",
		dma_sound_frequency(options),
		dma_sample_period(options),
		zc.x, zc.y, zc.z);
}

const char *verify(const struct audio *audio, const struct options *options)
{
	const struct zero_crossing zc = measure(audio, options);

	verify_assert (audio_duration(audio->format) >= 1.0)
		return "sample duration";

	verify_assert (zc.x == test_value(options).count[0])
		return "x count";

	verify_assert (zc.y == test_value(options).count[1])
		return "y count";

	verify_assert (zc.z == test_value(options).count[2])
		return "z count";

	return zc.error;
}
