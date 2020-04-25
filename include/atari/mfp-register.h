// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#ifndef ATARI_MFP_REGISTER_H
#define ATARI_MFP_REGISTER_H

#include "psgplay/types.h"

#define MFP_REGISTERS(mfp)						\
	mfp( 0, gpip,  GPIP,  "General-purpose I/O data register")	\
	mfp( 1, aer,   AER,   "Active edge register")			\
	mfp( 2, ddr,   DDR,   "Data direction register")		\
	mfp( 3, iera,  IERA,  "Interrupt enable register A")		\
	mfp( 4, ierb,  IERB,  "Interrupt enable register B")		\
	mfp( 5, ipra,  IPRA,  "Interrupt pending register A")		\
	mfp( 6, iprb,  IPRB,  "Interrupt pending register B")		\
	mfp( 7, isra,  ISRA,  "Interrupt in-service register A")	\
	mfp( 8, isrb,  ISRB,  "Interrupt in-service register B")	\
	mfp( 9, imra,  IMRA,  "Interrupt mask register A")		\
	mfp(10, imrb,  IMRB,  "Interrupt mask register B")		\
	mfp(11, vr,    VR,    "Vector register")			\
	mfp(12, tacr,  TACR,  "Timer A control register")		\
	mfp(13, tbcr,  TBCR,  "Timer B control register")		\
	mfp(14, tcdcr, TCDCR, "Timers C and D control register")	\
	mfp(15, tadr,  TADR,  "Timer A data register")			\
	mfp(16, tbdr,  TBDR,  "Timer B data register")			\
	mfp(17, tcdr,  TCDR,  "Timer C data register")			\
	mfp(18, tddr,  TDDR,  "Timer D data register")			\
	mfp(19, scr,   SCR,   "Synchronous character register")		\
	mfp(20, ucr,   UCR,   "USART control register")			\
	mfp(21, rsr,   RSR,   "Receiver status register")		\
	mfp(22, tsr,   TSR,   "Transmitter status register")		\
	mfp(23, udr,   UDR,   "USART data register")

enum mfp_reg {
#define MFP_REG_ENUM(register_, symbol_, label_, description_)		\
	MFP_REG_##label_ = register_,
MFP_REGISTERS(MFP_REG_ENUM)
};

#define DEFINE_MFP_PORT(type)						\
struct mfp_##type {							\
	__BITFIELD_FIELD(u8 mono_monitor_detect : 1,			\
	__BITFIELD_FIELD(u8 ring_indicator : 1,				\
	__BITFIELD_FIELD(u8 fdc_hdc : 1,				\
	__BITFIELD_FIELD(u8 acia : 1,					\
	__BITFIELD_FIELD(u8 blitter_done : 1,				\
	__BITFIELD_FIELD(u8 clear_to_send : 1,				\
	__BITFIELD_FIELD(u8 data_carrier_detect : 1,			\
	__BITFIELD_FIELD(u8 centronics_busy : 1,			\
	;))))))))							\
}

#define DEFINE_MFP_IRA(type)						\
struct mfp_##type {							\
	__BITFIELD_FIELD(u8 mono_monitor_detect : 1,			\
	__BITFIELD_FIELD(u8 ring_indicator : 1,				\
	__BITFIELD_FIELD(u8 timer_a : 1,				\
	__BITFIELD_FIELD(u8 rx_buffer_full : 1,				\
	__BITFIELD_FIELD(u8 rx_error : 1,				\
	__BITFIELD_FIELD(u8 tx_buffer_empty : 1,			\
	__BITFIELD_FIELD(u8 tx_error : 1,				\
	__BITFIELD_FIELD(u8 timer_b : 1,				\
	;))))))))							\
}

#define DEFINE_MFP_IRB(type)						\
struct mfp_##type {							\
	__BITFIELD_FIELD(u8 fdc_hdc : 1,				\
	__BITFIELD_FIELD(u8 acia : 1,					\
	__BITFIELD_FIELD(u8 timer_c : 1,				\
	__BITFIELD_FIELD(u8 timer_d : 1,				\
	__BITFIELD_FIELD(u8 blitter_ready : 1,				\
	__BITFIELD_FIELD(u8 clear_to_send : 1,				\
	__BITFIELD_FIELD(u8 data_carrier_detect : 1,			\
	__BITFIELD_FIELD(u8 centronics_busy : 1,			\
	;))))))))							\
}

enum mfp_ctrl {
	mfp_ctrl_stop,
	mfp_ctrl_div_4,
	mfp_ctrl_div_10,
	mfp_ctrl_div_16,
	mfp_ctrl_div_50,
	mfp_ctrl_div_64,
	mfp_ctrl_div_100,
	mfp_ctrl_div_200
};

#define DEFINE_MFP_TABCR(type)						\
struct mfp_##type {							\
	__BITFIELD_FIELD(u8 unused : 3,					\
	__BITFIELD_FIELD(u8 reset : 1,					\
	__BITFIELD_FIELD(u8 pulse_width_mode : 1,			\
	__BITFIELD_FIELD(u8 ctrl : 3,					\
	;))))								\
}

DEFINE_MFP_PORT(gpip);
DEFINE_MFP_PORT(aer);
DEFINE_MFP_PORT(ddr);
DEFINE_MFP_IRA(iera);
DEFINE_MFP_IRB(ierb);
DEFINE_MFP_IRA(ipra);
DEFINE_MFP_IRB(iprb);
DEFINE_MFP_IRA(isra);
DEFINE_MFP_IRB(isrb);
DEFINE_MFP_IRA(imra);
DEFINE_MFP_IRB(imrb);

struct mfp_vr {
	__BITFIELD_FIELD(u8 base : 4,
	__BITFIELD_FIELD(u8 sei : 1,
	__BITFIELD_FIELD(u8 unused : 3,
	;)))
};

DEFINE_MFP_TABCR(tacr);
DEFINE_MFP_TABCR(tbcr);

struct mfp_tcdcr {
	__BITFIELD_FIELD(u8 tc_unused : 1,
	__BITFIELD_FIELD(u8 tc_ctrl : 3,
	__BITFIELD_FIELD(u8 td_unused : 1,
	__BITFIELD_FIELD(u8 td_ctrl : 3,
	;))))
};

struct mfp_scr {
	__BITFIELD_FIELD(u8 reset : 1,
	__BITFIELD_FIELD(u8 data : 7,
	;))
};

struct mfp_ucr {
	__BITFIELD_FIELD(u8 clock_mode : 1,
	__BITFIELD_FIELD(u8 char_length : 2,
	__BITFIELD_FIELD(u8 format : 2,
	__BITFIELD_FIELD(u8 parity_enable : 1,
	__BITFIELD_FIELD(u8 parity : 1,
	__BITFIELD_FIELD(u8 unused : 1,
	;))))))
};

struct mfp_rsr {
	__BITFIELD_FIELD(u8 buffer_full : 1,
	__BITFIELD_FIELD(u8 overrun_error : 1,
	__BITFIELD_FIELD(u8 parity_error : 1,
	__BITFIELD_FIELD(u8 frame_error : 1,
	__BITFIELD_FIELD(u8 found_search_break_detect: 1,
	__BITFIELD_FIELD(u8 match_char_in_progress : 1,
	__BITFIELD_FIELD(u8 synch_strip_enable : 1,
	__BITFIELD_FIELD(u8 receiver_enable : 1,
	;))))))))
};

enum mfp_tsr_high_low {
	mfp_tsr_high_impedance,
	mfp_tsr_low,
	mfp_tsr_high,
	mfp_tsr_loopback_mode
};

struct mfp_tsr {
	__BITFIELD_FIELD(u8 buffer_empty : 1,
	__BITFIELD_FIELD(u8 underrun_error : 1,
	__BITFIELD_FIELD(u8 auto_turnaround : 1,
	__BITFIELD_FIELD(u8 end : 1,
	__BITFIELD_FIELD(u8 brk : 1,
	__BITFIELD_FIELD(u8 high_low : 2,
	__BITFIELD_FIELD(u8 transmitter_enable : 1,
	;)))))))
};

#define DEFINE_MFP_TDR(type)						\
struct mfp_##type {							\
	u8 count;							\
}

DEFINE_MFP_TDR(tadr);
DEFINE_MFP_TDR(tbdr);
DEFINE_MFP_TDR(tcdr);
DEFINE_MFP_TDR(tddr);

struct mfp_udr {
	u8 data;
};

#endif /* ATARI_MFP_REGISTER_H */
