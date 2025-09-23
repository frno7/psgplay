// SPDX-License-Identifier: GPL-2.0

#include <asm/snd/psg.h>
#include <asm/snd/sndh.h>

#include "test/sndhfrms.h"

sndh_title("FRMS");
sndh_tune_value_frames_names(int, tune_value_frames_names);
sndh_timer(SNDH_TIMER_C, 200);

static void idle()
{
	__asm__ __volatile__ ("stop #0x2200" : : : "cc");
}

static void idle_indefinitely()
{
	for (;;)
		idle();
}

void sndh_init(int tune)
{
	if (tune == 2)
		idle_indefinitely();

	snd_psg_wr_iomix(SND_PSG_IOMIX_OFF);
	snd_psg_wr_freq_a(440);
	snd_psg_wr_level_a(SND_PSG_LEVEL_MAX);
}

void sndh_play()
{
	snd_psg_wr_iomix(SND_PSG_IOMIX_TONE_A);
}

void sndh_exit()
{
	snd_psg_wr_iomix(SND_PSG_IOMIX_OFF);
}

