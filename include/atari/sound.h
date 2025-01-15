// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2020 Fredrik Noring
 */

#ifndef ATARI_SOUND_H
#define ATARI_SOUND_H

#include "atari/bus.h"
#include "atari/sample.h"

#define SOUND_FREQUENCY 2000000

#define SOUND_REGISTERS(reg)						\
	reg( 0, ctrl,    CTRL,    "DMA control")			\
	reg( 1, starthi, STARTHI, "Sample start address high byte")	\
	reg( 2, startmi, STARTMI, "Sample start address middle byte")	\
	reg( 3, startlo, STARTLO, "Sample start address low byte")	\
	reg( 4, counthi, COUNTHI, "Sample count address high byte")	\
	reg( 5, countmi, COUNTMI, "Sample count address middle byte")	\
	reg( 6, countlo, COUNTLO, "Sample count address low byte")	\
	reg( 7, endhi,   ENDHI,   "Sample start address high byte")	\
	reg( 8, endmi,   ENDMI,   "Sample start address middle byte")	\
	reg( 9, endlo,   ENDLO,   "Sample start address low byte")	\
	reg(16, mode,    MODE,    "Sound mode control")

enum sound_reg {
#define SOUND_REG_ENUM(register_, symbol_, label_, description_)	\
	SOUND_REG_##label_ = register_,
SOUND_REGISTERS(SOUND_REG_ENUM)
};

struct sound_ctrl {
	__BITFIELD_FIELD(u8 : 6,
	__BITFIELD_FIELD(u8 loop : 1,
	__BITFIELD_FIELD(u8 dma : 1,
	;)))
};

#define SOUND_SAMPLE_FREQUENCY(f)					\
	f(6258)								\
	f(12517)							\
	f(25033)							\
	f(50066)

enum sound_sample_frequency {
#define SOUND_SAMPLE_FREQUENCY_ENUM(f)					\
	sound_sample_frequency_##f,
SOUND_SAMPLE_FREQUENCY(SOUND_SAMPLE_FREQUENCY_ENUM)
};

struct sound_mode {
	__BITFIELD_FIELD(u8 mono : 1,
	__BITFIELD_FIELD(u8 : 5,
	__BITFIELD_FIELD(u8 sample_frequency : 2,
	;)))
};

union sound {
	struct {
		struct sound_ctrl ctrl;
		u8 starthi;
		u8 startmi;
		u8 startlo;
		u8 counthi;
		u8 countmi;
		u8 countlo;
		u8 endhi;
		u8 endmi;
		u8 endlo;
		u8 unused[6];
		struct sound_mode mode;
	};
	u8 reg[17];
};

extern const struct device sound_device;

void sound_sample(sound_sample_f sample, void *sample_arg);

void sound_check(u32 bus_address);

#endif /* ATARI_SOUND_H */
