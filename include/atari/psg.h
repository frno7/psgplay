// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#ifndef ATARI_PSG_H
#define ATARI_PSG_H

#include "atari/bus.h"
#include "atari/sample.h"

#define PSG_FREQUENCY 2000000

#define PSG_REGISTERS(psg)						\
	psg( 0, plo_a,   PLO_A,   "Period of channel A fine tone")	\
	psg( 1, phi_a,   PHI_A,   "Period of channel A rough tone")	\
	psg( 2, plo_b,   PLO_B,   "Period of channel B fine tone")	\
	psg( 3, phi_b,   PHI_B,   "Period of channel B rough tone")	\
	psg( 4, plo_c,   PLO_C,   "Period of channel C fine tone")	\
	psg( 5, phi_c,   PHI_C,   "Period of channel C rough tone")	\
	psg( 6, noise,   NOISE,   "Period of noise")			\
	psg( 7, iomix,   IOMIX,   "I/O port and mixer settings")	\
	psg( 8, level_a, LEVEL_A, "Level of channel A")			\
	psg( 9, level_b, LEVEL_B, "Level of channel B")			\
	psg(10, level_c, LEVEL_C, "Level of channel C")			\
	psg(11, plo_env, PLO_ENV, "Period of envelope fine")		\
	psg(12, phi_env, PHI_ENV, "Period of envelope rough")		\
	psg(13, shape,   SHAPE,   "Shape of envelope")			\
	psg(14, data_a,  DATA_A,  "Data of I/O port A")			\
	psg(15, data_b,  DATA_B,  "Data of I/O port B")

enum psg_reg {
#define PSG_REG_ENUM(register_, symbol_, label_, description_)		\
	PSG_REG_##label_ = register_,
PSG_REGISTERS(PSG_REG_ENUM)
};

struct psg_channel_lo {
	u8 period;
};

struct psg_channel_hi {
	__BITFIELD_FIELD(u8 : 4,
	__BITFIELD_FIELD(u8 period : 4,
	;))
};

struct psg_noise {
	__BITFIELD_FIELD(u8 : 3,
	__BITFIELD_FIELD(u8 period : 5,
	;))
};

struct psg_iomix {
	__BITFIELD_FIELD(u8 io_b : 1,
	__BITFIELD_FIELD(u8 io_a : 1,
	__BITFIELD_FIELD(u8 noise_c : 1,
	__BITFIELD_FIELD(u8 noise_b : 1,
	__BITFIELD_FIELD(u8 noise_a : 1,
	__BITFIELD_FIELD(u8 tone_c : 1,
	__BITFIELD_FIELD(u8 tone_b : 1,
	__BITFIELD_FIELD(u8 tone_a : 1,
	;))))))))
};

struct psg_level {
	__BITFIELD_FIELD(u8 : 3,
	__BITFIELD_FIELD(u8 m : 1,
	__BITFIELD_FIELD(u8 level : 4,
	;)))
};

struct psg_envelope_lo {
	u8 period;
};

struct psg_envelope_hi {
	u8 period;
};

struct psg_envelope_shape {
	union {
		__BITFIELD_FIELD(u8 : 4,
		__BITFIELD_FIELD(u8 cont : 1,
		__BITFIELD_FIELD(u8 att : 1,
		__BITFIELD_FIELD(u8 alt : 1,
		__BITFIELD_FIELD(u8 hold: 1,
		;)))))
		__BITFIELD_FIELD(u8 : 4,
		__BITFIELD_FIELD(u8 ctrl : 4,
		;))
	};
};

struct psg_data_a {
	__BITFIELD_FIELD(u8 ide_drive_enable : 1,
	__BITFIELD_FIELD(u8 monitor_jack_gpo : 1,
	__BITFIELD_FIELD(u8 centronics_strobe : 1,
	__BITFIELD_FIELD(u8 data_terminal_ready : 1,
	__BITFIELD_FIELD(u8 ready_to_send : 1,
	__BITFIELD_FIELD(u8 floppy_select_1 : 1,
	__BITFIELD_FIELD(u8 floppy_select_0 : 1,
	__BITFIELD_FIELD(u8 floppy_select_side : 1,
	;))))))))
};

struct psg_data_b {
	u8 centronics_data;
};

union psg {
	struct {
		struct psg_channel_lo lo_a;
		struct psg_channel_hi hi_a;
		struct psg_channel_lo lo_b;
		struct psg_channel_hi hi_b;
		struct psg_channel_lo lo_c;
		struct psg_channel_hi hi_c;
		struct psg_noise noise;
		struct psg_iomix iomix;
		struct psg_level level_a;
		struct psg_level level_b;
		struct psg_level level_c;
		struct psg_envelope_lo envelope_lo;
		struct psg_envelope_hi envelope_hi;
		struct psg_envelope_shape envelope_shape;
		struct psg_data_a data_a;
		struct psg_data_b data_b;
	};
	u8 reg[16];
};

extern const struct device psg_device;

void psg_sample(psg_sample_f sample, void *sample_arg);

#endif /* ATARI_PSG_H */
