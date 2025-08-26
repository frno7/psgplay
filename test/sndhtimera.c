// SPDX-License-Identifier: GPL-2.0

#include <asm/snd/sndh.h>

#include "test/sndhtimera.h"

sndh_title("SNDH timer A " XSTR(SNDH_TIMER_FREQUENCY) " Hz");
sndh_timer(SNDH_TIMER_A, SNDH_TIMER_FREQUENCY);

#include "test/sndhtimer.h"
