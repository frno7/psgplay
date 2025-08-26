// SPDX-License-Identifier: GPL-2.0

#include <asm/snd/sndh.h>

#include "test/sndhtimerb.h"

sndh_title("SNDH timer B " XSTR(SNDH_TIMER_FREQUENCY) " Hz");
sndh_timer(SNDH_TIMER_B, SNDH_TIMER_FREQUENCY);

#include "test/sndhtimer.h"
