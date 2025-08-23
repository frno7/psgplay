// SPDX-License-Identifier: GPL-2.0

#include <asm/snd/psg.h>
#include <asm/sndh.h>

#include "test/pitch.h"

sndh_title("Pitch");
sndh_tune_value_names(int, tune_value_names);

void sndh_init(int tune)
{
	const int pitch = sndh_tune_select_value(tune);

	snd_psg_wr_iomix(SND_PSG_IOMIX_OFF);
	snd_psg_wr_freq_a(pitch);
	snd_psg_wr_level_a(15);
}

void sndh_play()
{
	snd_psg_wr_iomix(SND_PSG_IOMIX_TONE_A);
}

void sndh_exit()
{
	snd_psg_wr_iomix(SND_PSG_IOMIX_OFF);
}
