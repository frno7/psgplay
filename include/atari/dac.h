// SPDX-License-Identifier: GPL-2.0

#ifndef ATARI_DAC_H
#define ATARI_DAC_H

#include "internal/types.h"

struct cf2149_dac {
	uint16_t lvl[32][32][32];
};

void cf2149_atari_st_dac(struct cf2149_dac *dac);

#endif /* ATARI_DAC_H */
