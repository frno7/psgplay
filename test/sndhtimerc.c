// SPDX-License-Identifier: GPL-2.0

#include <asm/snd/sndh.h>

#include "test/sndhtimerc.h"

sndh_title("SNDH timer C " XSTR(SNDH_TIMER_FREQUENCY) " Hz");
sndh_timer(SNDH_TIMER_C, SNDH_TIMER_FREQUENCY);

#include "test/sndhtimer.h"
