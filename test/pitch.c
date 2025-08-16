// SPDX-License-Identifier: GPL-2.0

#include <asm/psg.h>
#include <asm/sndh.h>

#define tune_value_names(t)						\
	t(440, "A4 pitch standard A440 440 Hz")				\
	t(262, "C4 middle C 262 Hz")

sndh_title("Pitch");
sndh_tune_value_names(int, tune_value_names);

void sndh_init(int tune)
{
	const int pitch = sndh_tune_select_value(tune);

	psg_wr_freq_a(pitch);
	psg_wr_level_a(15);
}

void sndh_play()
{
	psg_wr_iomix(PSG_IOMIX_TONE_A);
}

void sndh_exit()
{
	psg_wr_iomix(PSG_IOMIX_OFF);
}
