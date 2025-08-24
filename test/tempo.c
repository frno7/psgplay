// SPDX-License-Identifier: GPL-2.0

#include <asm/snd/sndh.h>
#include <asm/interrupt.h>
#include <asm/io.h>

#include "atari/mfp-map.h"

#include "test/tempo.h"
#include "test/tempo-dma.h"

sndh_title("Tempo");
sndh_tune_value_names(struct timer_preset, tune_value_names);
sndh_timer(SNDH_TIMER_B, 16);

static struct tempo_sample sample[1];
static struct timer_preset preset;

static INTERRUPT void vbl_play()
{
	tempo_play(sample);
}

static INTERRUPT void timer_a_play()
{
	tempo_play(sample);

	barrier();

	mfp_map()->isra.timer_a = false;

	barrier();
}

void sndh_init(int tune)
{
	preset = sndh_tune_select_value(tune);

	tempo_init(sample);

	if (preset.frequency == 50) {
		iowr32((uint32_t)vbl_play, 0x70);
	} else if (preset.ctrl) {
		iowr32((uint32_t)timer_a_play, 0x134);

		barrier();

		mfp_map()->tacr.ctrl = preset.ctrl;
		mfp_map()->tadr.count = preset.count;
		mfp_map()->iera.timer_a = true;
		mfp_map()->ipra.timer_a = false;
		mfp_map()->isra.timer_a = false;
		mfp_map()->imra.timer_a = true;

		barrier();
	}
}

void sndh_play()
{
	if (!preset.ctrl && preset.frequency == 16)
		tempo_play(sample);
}

void sndh_exit()
{
	tempo_exit(sample);

	// FIXME: Restore vectors and MFP
}
