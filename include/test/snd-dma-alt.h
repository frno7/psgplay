// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2025 Fredrik Noring
 */

#ifndef PSGPLAY_TEST_SND_DMA_ALT_H
#define PSGPLAY_TEST_SND_DMA_ALT_H

#include <stdbool.h>

#include <asm/snd/dma.h>

struct snd_dma_alt_sample {
	union {
		struct {
			int8_t left;
			int8_t right;
		};
		int16_t u16;
	};
};

static inline void snd_dma_alt_init(struct snd_dma_alt_sample *sample)
{
	snd_dma_wr_start(&sample[0]);
	snd_dma_wr_end(&sample[1]);

	snd_dma_wrs_mode({
		.mode = SND_DMA_MODE_STEREO8,
		.frequency = SND_DMA_FREQUENCY_50066
	});
}

static inline void snd_dma_alt_play(struct snd_dma_alt_sample *sample)
{
	if (!sample[0].u16) {
		sample[0].left  = 0x7f;
		sample[0].right = 0x7f;

		snd_dma_wrs_ctrl({ .play_repeat = true, .play = true });
	} else
		sample[0].u16 = ~sample[0].u16;
}

static inline void snd_dma_alt_exit(struct snd_dma_alt_sample *sample)
{
	snd_dma_wrs_ctrl({ .play = false });
}

#endif /* PSGPLAY_TEST_SND_DMA_ALT_H */
