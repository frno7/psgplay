// SPDX-License-Identifier: GPL-2.0

#include <stdbool.h>
#include <stddef.h>

#include <asm/snd/dma.h>
#include <asm/snd/sndh.h>

#include "test/dmapitch.h"

sndh_title("DMA pitch");
sndh_tune_value_names(struct dma_preset, tune_value_names);

#define R1(x)	x
#define R2(x)	x, x
#define R3(x)	x, x, x
#define R5(x)	x, x, x, x, x
#define R7(x)	x, x, x, x, x, x, x

struct mono {
	int8_t value;
};

struct stereo {
	int8_t left;
	int8_t right;
};

#define M(hp)								\
static const struct mono mono##hp[2*hp] = { R##hp({127}), R##hp({-127}) }

#define S(hp)								\
static const struct stereo stereo##hp[2*hp] = { R##hp({R2(127)}), R##hp({R2(-127)}) }

#define SM(hp) M(hp); S(hp)

SM(1);
SM(2);
SM(3);
SM(5);
SM(7);

void sndh_init(int tune)
{
	const struct dma_preset preset = sndh_tune_select_value(tune);
	const int8_t *start = NULL;

	switch ((preset.channels << 8) | preset.halfperiod) {
	case 0x101: start = &mono1[0].value;  break;
	case 0x102: start = &mono2[0].value;  break;
	case 0x103: start = &mono3[0].value;  break;
	case 0x105: start = &mono5[0].value;  break;
	case 0x107: start = &mono7[0].value;  break;
	case 0x201: start = &stereo1[0].left; break;
	case 0x202: start = &stereo2[0].left; break;
	case 0x203: start = &stereo3[0].left; break;
	case 0x205: start = &stereo5[0].left; break;
	case 0x207: start = &stereo7[0].left; break;
	}

	const int8_t *end = &start[2 * preset.halfperiod * preset.channels];

	snd_dma_wr_start(start);
	snd_dma_wr_end(end);
	snd_dma_wrs_mode({
		.mode = preset.channels == 1 ?
			SND_DMA_MODE_MONO8 :
			SND_DMA_MODE_STEREO8,
		.frequency = preset.frequency,
	});
}

void sndh_play()
{
	if (!snd_dma_rd_ctrl().play)
		snd_dma_wrs_ctrl({ .play_repeat = true, .play = true });
}

void sndh_exit()
{
	snd_dma_wrs_ctrl({ .play = false });
}
