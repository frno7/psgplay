// SPDX-License-Identifier: GPL-2.0

#include <stdbool.h>
#include <stddef.h>

#include <asm/snd/dma.h>
#include <asm/snd/sndh.h>

#include "test/dmapitch.h"

sndh_title("DMA pitch");
sndh_tune_value_time_names(struct dma_preset, tune_value_time_names);

#define R1(x)	x
#define R2(x)	x, x
#define R3(x)	x, x, x
#define R5(x)	x, x, x, x, x
#define R7(x)	x, x, x, x, x, x, x

struct mono {
	int8_t mono;
};

struct stereo {
	int8_t left;
	int8_t right;
};

#define M(hp)								\
static const struct mono samples1_##hp[2*hp] = {			\
	R##hp({ 127}),							\
	R##hp({-127})							\
}

#define S(hp)								\
static const struct stereo samples2_##hp[2*hp] = {			\
	R##hp({R2( 127)}),						\
	R##hp({R2(-127)})						\
}

#define SM(hp) M(hp); S(hp)

SM(1);
SM(2);
SM(3);
SM(5);
SM(7);

void sndh_init(int tune)
{
	const struct dma_preset preset = sndh_tune_select_value(tune);
	const int8_t *base = NULL;

#define CASE(c, hp, f) ((c) << 8) | (hp): base = &samples##c##_##hp[0].f

	switch ((preset.channels << 8) | preset.halfperiod) {
	case CASE(1, 1, mono); break;
	case CASE(1, 2, mono); break;
	case CASE(1, 3, mono); break;
	case CASE(1, 5, mono); break;
	case CASE(1, 7, mono); break;
	case CASE(2, 1, left); break;
	case CASE(2, 2, left); break;
	case CASE(2, 3, left); break;
	case CASE(2, 5, left); break;
	case CASE(2, 7, left); break;
	}

	const int8_t *end = &base[2 * preset.halfperiod * preset.channels];

	snd_dma_wr_base(base);
	snd_dma_wr_end(end);
	snd_dma_wrs_mode({
		.format = preset.channels == 1 ?
			SND_DMA_MODE_FORMAT_MONO8 :
			SND_DMA_MODE_FORMAT_STEREO8,
		.rate = preset.rate,
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
