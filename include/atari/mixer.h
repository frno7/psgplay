// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2020 Fredrik Noring
 */

#ifndef ATARI_MIXER_H
#define ATARI_MIXER_H

#include "atari/bus.h"

#define MIXER_MICROWIRE_REGISTERS(reg)					\
	reg(0, data, DATA, "Microwire mixer data")			\
	reg(1, mask, MASK, "Microwire mixer mask")

enum mixer_microwire_reg {
#define MIXER_MICROWIRE_REG_ENUM(register_, symbol_, label_, description_)\
	MIXER_MICROWIRE_REG_##label_ = register_,
MIXER_MICROWIRE_REGISTERS(MIXER_MICROWIRE_REG_ENUM)
};

#define MIXER_LCM1992_REGISTERS(reg)					\
	reg(0, mixer,        MIXER,        "Mixer")			\
	reg(1, bass,         BASS,         "Bass")			\
	reg(2, treble,       TREBLE,       "Treble")			\
	reg(3, volume_main,  VOLUME_MAIN,  "Main volume")		\
	reg(4, volume_right, VOLUME_RIGHT, "Right volume")		\
	reg(5, volume_left,  VOLUME_LEFT,  "Left volume")

enum mixer_lcm1992_reg {
#define MIXER_LCM1992_REG_ENUM(register_, symbol_, label_, description_)\
	MIXER_LCM1992_REG_##label_ = register_,
MIXER_LCM1992_REGISTERS(MIXER_LCM1992_REG_ENUM)
};

extern const struct device mixer_device;

void mixer_sample(mixer_sample_f sample, void *sample_arg);

#endif /* ATARI_MIXER_H */
