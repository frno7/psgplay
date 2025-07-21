// SPDX-License-Identifier: GPL-2.0

#ifndef ATARI_DAC_H
#define ATARI_DAC_H

#include "internal/types.h"

struct cf2149_dac {
	uint16_t lvl[32][32][32];
};

const struct cf2149_dac *cf2149_atari_st_dac();

#endif /* ATARI_DAC_H */
