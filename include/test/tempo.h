// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2025 Fredrik Noring
 */

#ifndef PSGPLAY_TEST_TEMPO_H
#define PSGPLAY_TEST_TEMPO_H

#include <stdbool.h>

#include <asm/snd/dma.h>

#define tempo_tune_value_names(t)					\
	t(0, "50 Hz VBL half period (25 Hz)")

struct tempo_sample {
	union {
		struct {
			uint8_t left;
			uint8_t right;
		};
		uint16_t u16;
	};
};

static inline void tempo_init(struct tempo_sample *sample)
{
	snd_dma_wr_start(&sample[0]);
	snd_dma_wr_end(&sample[1]);

	snd_dma_wrs_mode({
		.mode = SND_DMA_MODE_STEREO8,
		.frequency = SND_DMA_FREQUENCY_50066
	});
}

static inline void tempo_play(struct tempo_sample *sample)
{
	if (!sample[0].u16) {
		sample[0].left  = 0x7f;
		sample[0].right = 0x7f;

		snd_dma_wrs_ctrl({ .play_repeat = true, .play = true });
	} else
		sample[0].u16 = ~sample[0].u16;
}

static inline void tempo_exit(struct tempo_sample *sample)
{
	snd_dma_wrs_ctrl({ .play = false });
}

#endif /* PSGPLAY_TEST_TEMPO_H */
