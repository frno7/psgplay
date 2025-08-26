// SPDX-License-Identifier: GPL-2.0

#include <asm/snd/sndh.h>

#include "test/sndhtimervbl.h"

sndh_title("SNDH timer VBL " XSTR(SNDH_TIMER_FREQUENCY) " Hz");
sndh_timer(SNDH_TIMER_VBL, SNDH_TIMER_FREQUENCY);

#include "test/sndhtimer.h"
