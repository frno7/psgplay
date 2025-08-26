// SPDX-License-Identifier: GPL-2.0

#include <asm/snd/sndh.h>
#include <asm/interrupt.h>
#include <asm/io.h>
#include <asm/mfp.h>

#include "internal/build-assert.h"

#include "test/snd-dma-alt.h"
#include "test/tempo.h"

sndh_title("Tempo");
sndh_tune_value_names(struct timer_preset, tune_value_names);

static struct snd_dma_alt_sample sample[1];

static INTERRUPT void timer_a_play()
{
	snd_dma_alt_play(sample);

	mfp_clrs_isra({ .timer_a = true });
}

void sndh_init(int tune)
{
	const struct timer_preset preset = sndh_tune_select_value(tune);

	snd_dma_alt_init(sample);

	iowr32((uint32_t)timer_a_play, 0x134);

	BUILD_BUG_ON(sizeof(struct mfp_ier) != 2);
	BUILD_BUG_ON(sizeof(struct mfp_ipr) != 2);
	BUILD_BUG_ON(sizeof(struct mfp_isr) != 2);
	BUILD_BUG_ON(sizeof(struct mfp_imr) != 2);

	mfp_wrs_tacr({ .ctrl = preset.ctrl });
	mfp_wrs_tadr({ .count = preset.count });
	mfp_sets_ier({ .timer_a = true });
	mfp_clrs_ipr({ .timer_a = true });
	mfp_clrs_isr({ .timer_a = true });
	mfp_sets_imr({ .timer_a = true });
}

void sndh_play()
{
}

void sndh_exit()
{
	snd_dma_alt_exit(sample);

	// FIXME: Restore vectors and MFP
}
