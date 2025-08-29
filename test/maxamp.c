// SPDX-License-Identifier: GPL-2.0

#include <stdbool.h>

#include <asm/snd/dma.h>
#include <asm/snd/psg.h>
#include <asm/snd/sndh.h>

#include "internal/macro.h"

#include "test/maxamp.h"

sndh_title("PSG and DMA maximum amplitude square waves");
sndh_tune_value_names(int, tune_value_names);
sndh_timer(SNDH_TIMER_A, 400);

#define R8(x) x, x, x, x, x, x, x, x,
#define R64(x) R8(x) R8(x) R8(x) R8(x) R8(x) R8(x) R8(x) R8(x)

static const int8_t samples[] = { R64(+127) R64(-128) };

void sndh_init(int tune)
{
	snd_psg_wr_iomix(SND_PSG_IOMIX_OFF);
	snd_psg_wr_period_a(73);
	snd_psg_wr_period_b(103);
	snd_psg_wr_period_c(173);
	snd_psg_wr_level_a(SND_PSG_LEVEL_MAX);
	snd_psg_wr_level_b(SND_PSG_LEVEL_MAX);
	snd_psg_wr_level_c(SND_PSG_LEVEL_MAX);

	snd_dma_wr_start(&samples[0]);
	snd_dma_wr_end(&samples[ARRAY_SIZE(samples)]);

	snd_dma_wrs_mode({
		.mode = SND_DMA_MODE_MONO8,
		.frequency = SND_DMA_FREQUENCY_6258
	});
}

void sndh_play()
{
	snd_psg_wr_iomix(SND_PSG_IOMIX_TONE_A &
			 SND_PSG_IOMIX_TONE_B &
			 SND_PSG_IOMIX_TONE_C);

	snd_dma_wrs_ctrl({ .play_repeat = true, .play = true });
}

void sndh_exit()
{
	snd_psg_wr_iomix(SND_PSG_IOMIX_OFF);

	snd_dma_wrs_ctrl({ .play = false });
}
