// SPDX-License-Identifier: GPL-2.0

#include <asm/snd/sndh.h>

#include "test/tempo.h"

sndh_title("Tempo");
sndh_tune_value_names(int, tempo_tune_value_names);
sndh_timer(SNDH_TIMER_VBL, 50);

static struct tempo_sample sample[1];

void sndh_init(int tune)
{
	tempo_init(sample);
}

void sndh_play()
{
	tempo_play(sample);
}

void sndh_exit()
{
	tempo_exit(sample);
}
