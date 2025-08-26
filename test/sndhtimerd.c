// SPDX-License-Identifier: GPL-2.0

#include <asm/snd/sndh.h>

#include "test/sndhtimerd.h"

sndh_title("SNDH timer D " XSTR(SNDH_TIMER_FREQUENCY) " Hz");
sndh_timer(SNDH_TIMER_D, SNDH_TIMER_FREQUENCY);

#include "test/sndhtimer.h"
