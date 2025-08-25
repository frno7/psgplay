// SPDX-License-Identifier: GPL-2.0

#include <asm/snd/sndh.h>
#include <asm/interrupt.h>
#include <asm/io.h>
#include <asm/mfp.h>

#include "test/snd-dma-alt.h"
#include "test/tempo.h"

sndh_title("Tempo");
sndh_tune_value_names(struct timer_preset, tune_value_names);
sndh_timer(SNDH_TIMER_B, 16);

static struct snd_dma_alt_sample sample[1];
static struct timer_preset preset;

static INTERRUPT void vbl_play()
{
	snd_dma_alt_play(sample);
}

static INTERRUPT void timer_a_play()
{
	snd_dma_alt_play(sample);

	mfp_clrs_isra({ .timer_a = true });
}

void sndh_init(int tune)
{
	preset = sndh_tune_select_value(tune);

	snd_dma_alt_init(sample);

	if (preset.frequency == 50) {
		iowr32((uint32_t)vbl_play, 0x70);
	} else if (preset.ctrl) {
		iowr32((uint32_t)timer_a_play, 0x134);

		mfp_wrs_tacr({ .ctrl = preset.ctrl });
		mfp_wrs_tadr({ .count = preset.count });
		mfp_sets_iera({ .timer_a = true });
		mfp_clrs_ipra({ .timer_a = true });
		mfp_clrs_isra({ .timer_a = true });
		mfp_sets_imra({ .timer_a = true });
	}
}

void sndh_play()
{
	if (!preset.ctrl && preset.frequency == 16)
		snd_dma_alt_play(sample);
}

void sndh_exit()
{
	snd_dma_alt_exit(sample);

	// FIXME: Restore vectors and MFP
}
