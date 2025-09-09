// SPDX-License-Identifier: GPL-2.0

#include <stdbool.h>
#include <stddef.h>

#include <asm/interrupt.h>
#include <asm/io.h>
#include <asm/mfp.h>
#include <asm/snd/dma.h>
#include <asm/snd/sndh.h>

#include "test/dmasint.h"

sndh_title("DMA SINT");
sndh_tune_value_time_names(struct dma_preset, tune_value_time_names);
sndh_timer(SNDH_TIMER_C, 200);

#define R2(...)		__VA_ARGS__, __VA_ARGS__
#define R4(...)		R2(R2(__VA_ARGS__))
#define R8(...)		R2(R4(__VA_ARGS__))
#define R9(...)		R8(__VA_ARGS__), __VA_ARGS__
#define R10(...)	R9(__VA_ARGS__), __VA_ARGS__
#define R11(...)	R10(__VA_ARGS__), __VA_ARGS__
#define R32(...)	R4(R8(__VA_ARGS__))
#define R64(...)	R2(R32(__VA_ARGS__))
#define R128(...)	R2(R64(__VA_ARGS__))

struct mono {
	int8_t mono;
};

struct stereo {
	int8_t left;
	int8_t right;
};

#define DM(hp, x, y, z)		/* (hp+1) for zero ending */		\
static const struct mono samples1_##hp[3][2*(hp+1)] = {			\
		{ R##hp({x}), R##hp({-x}) },				\
		{ R##hp({y}), R##hp({-y}) },				\
		{ R##hp({z}), R##hp({-z}) }				\
	}

#define DS(hp, x, y, z)		/* (hp+1) for zero ending */		\
static const struct stereo samples2_##hp[3][2*(hp+1)] = {		\
		{ R##hp({R2(x)}), R##hp({R2(-x)}) },			\
		{ R##hp({R2(y)}), R##hp({R2(-y)}) },			\
		{ R##hp({R2(z)}), R##hp({R2(-z)}) }			\
	}

#define DSM(hp) DM(hp, 31, 63, 127); DS(hp, 31, 63, 127)

DSM(8);
DSM(9);
DSM(10);
DSM(11);
DSM(32);
DSM(64);
DSM(128);

static struct dma_preset preset;

static struct frame {
	const int8_t *base;
	const int8_t *end;
	int count;
} frames[4];

struct frame *frame = &frames[0];

#define ZERO_ENDING 2

static INTERRUPT void play_timer_a_end()
{
	mfp_clrs_isr({ .timer_a = true });
}

static INTERRUPT void play_timer_a_2()
{
	mfp_clrs_isr({ .timer_a = true });

	mfp_wrs_tacr({ });	/* Stop event count timer */

	iowr32((uint32_t)play_timer_a_end, 0x134);

	snd_dma_wrs_ctrl({ .play = true });

	snd_dma_wr_end(frame->end + ZERO_ENDING);
}

static INTERRUPT void play_timer_a_1()
{
	mfp_clrs_isr({ .timer_a = true });

	mfp_wrs_tacr({ });	/* Stop event count timer */

	snd_dma_wr_base(frame->base);

	if (frame->count > 1) {
		iowr32((uint32_t)play_timer_a_2, 0x134);

		mfp_wrs_tadr({ .count = frame->count - 1 });
		mfp_wrs_tacr({ .event = true });

		snd_dma_wr_end(frame->end);
	} else {
		iowr32((uint32_t)play_timer_a_end, 0x134);

		snd_dma_wr_end(frame->end + ZERO_ENDING);

		snd_dma_wrs_ctrl({ .play = true });
	}
}

static INTERRUPT void play_timer_a_0()
{
	mfp_clrs_isr({ .timer_a = true });

	snd_dma_wr_base(frame->base);
	snd_dma_wr_end(frame->end);

	iowr32((uint32_t)play_timer_a_1, 0x134);

	mfp_wrs_tacr({ });	/* Stop event count timer */
	mfp_wrs_tadr({ .count = frame->count });
	mfp_wrs_tacr({ .event = true });

	frame++;
}

static INTERRUPT void play_gpip7_end()
{
	mfp_clrs_isr({ .gpip7 = true });
}

static INTERRUPT void play_gpip7_2()
{
	mfp_clrs_isr({ .gpip7 = true });

	if (--frame->count > 1)
		return;

	iowr32((uint32_t)play_gpip7_end, 0x13c);

	snd_dma_wrs_ctrl({ .play = true });

	snd_dma_wr_end(frame->end + ZERO_ENDING);
}

static INTERRUPT void play_gpip7_1()
{
	mfp_clrs_isr({ .gpip7 = true });

	if (--frame->count > 0)
		return;

	frame++;

	snd_dma_wr_base(frame->base);

	if (frame->count > 1) {
		iowr32((uint32_t)play_gpip7_2, 0x13c);

		snd_dma_wr_end(frame->end);
	} else {
		iowr32((uint32_t)play_gpip7_end, 0x13c);

		snd_dma_wr_end(frame->end + ZERO_ENDING);

		snd_dma_wrs_ctrl({ .play = true });
	}
}

static INTERRUPT void play_gpip7_0()
{
	mfp_clrs_isr({ .gpip7 = true });

	if (--frame->count > 1)
		return;

	frame++;

	snd_dma_wr_base(frame->base);
	snd_dma_wr_end(frame->end);

	iowr32((uint32_t)play_gpip7_1, 0x13c);
}

void sndh_init(int tune)
{
	preset = sndh_tune_select_value(tune);
	const int8_t *base = NULL;

#define CASE(c, hp, f) ((c) << 8) | (hp): base = &samples##c##_##hp[0][0].f

	switch ((preset.channels << 8) | preset.halfperiod) {
	case CASE(1,   8, mono); break;
	case CASE(1,   9, mono); break;
	case CASE(1,  10, mono); break;
	case CASE(1,  11, mono); break;
	case CASE(1,  32, mono); break;
	case CASE(1,  64, mono); break;
	case CASE(1, 128, mono); break;

	case CASE(2,   8, left); break;
	case CASE(2,   9, left); break;
	case CASE(2,  10, left); break;
	case CASE(2,  11, left); break;
	case CASE(2,  32, left); break;
	case CASE(2,  64, left); break;
	case CASE(2, 128, left); break;
	}

			     /* (preset.halfperiod+1) for zero ending */
#define BASE(n) &base[(n) * 2 * (preset.halfperiod+1) * preset.channels]
			     /* preset.halfperiod without zero ending */
#define END(n)  &((BASE(n))[2 * preset.halfperiod * preset.channels])
#define FRAME(n) frames[n] = (struct frame) {				\
		.base = BASE(n),					\
		.end = END(n),						\
		.count = preset.count[n],				\
	}

	FRAME(0);
	FRAME(1);
	FRAME(2);

	if (preset.timer_a) {
		/* FIXME: data direction etc. */
		mfp_clrs_ier({ .timer_a = true });
		mfp_clrs_ipr({ .timer_a = true });
		mfp_clrs_isr({ .timer_a = true });

		mfp_sets_imr({ .timer_a = true });
		mfp_sets_ier({ .timer_a = true });
	} else {
		mfp_clrs_ier({ .gpip7 = true });
		mfp_clrs_ipr({ .gpip7 = true });
		mfp_clrs_isr({ .gpip7 = true });

		mfp_sets_aer({ .gpip7 = true });  /* For a colour monitor */

		mfp_sets_imr({ .gpip7 = true });
		mfp_sets_ier({ .gpip7 = true });
	}

	snd_dma_wrs_mode({
		.format = preset.channels == 1 ?
			SND_DMA_MODE_FORMAT_MONO8 :
			SND_DMA_MODE_FORMAT_STEREO8,
		.rate = preset.rate,
	});
}

static void play_timer_a()
{
	if (frame->count == 1) {
		iowr32((uint32_t)play_timer_a_1, 0x134);

		frame++;

		mfp_wrs_tacr({ });	/* Stop event count timer */
		mfp_wrs_tadr({ .count = frame->count });
		mfp_wrs_tacr({ .event = true });

		snd_dma_wrs_ctrl({ .play_repeat = true, .play = true });

		snd_dma_wr_base(frame->base);
		snd_dma_wr_end(frame->end);

		frame++;
	} else {
		iowr32((uint32_t)play_timer_a_0, 0x134);

		mfp_wrs_tacr({ });	/* Stop event count timer */
		mfp_wrs_tadr({ .count = frame->count - 1 });
		mfp_wrs_tacr({ .event = true });

		snd_dma_wrs_ctrl({ .play_repeat = true, .play = true });

		frame++;
	}
}

static void play_gpip7()
{
	if (frame->count == 1) {
		iowr32((uint32_t)play_gpip7_1, 0x13c);

		frame++;

		snd_dma_wrs_ctrl({ .play_repeat = true, .play = true });

		snd_dma_wr_base(frame->base);
		snd_dma_wr_end(frame->end);
	} else {
		iowr32((uint32_t)play_gpip7_0, 0x13c);

		snd_dma_wrs_ctrl({ .play_repeat = true, .play = true });
	}
}

void sndh_play()
{
	static int n = 0;

	if (n++ != 250)
		return;

	/* After slightly more than a second... */

	snd_dma_wr_base(frame->base);
	snd_dma_wr_end(frame->end);

	(preset.timer_a ? play_timer_a : play_gpip7)();
}

void sndh_exit()
{
	snd_dma_wrs_ctrl({ .play = false });
}
