// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#ifndef ATARI_SHIFTER_H
#define ATARI_SHIFTER_H

#include "atari/bus.h"

#define SHIFTER_REGISTERS(sh)						\
	sh( 0, vbasehi,   VBASEHI,   "Video display base high byte")	\
	sh( 1, vbasemid,  VBASEMID,  "Video display base mid byte")	\
	sh( 2, vcounthi,  VCOUNTHI,  "Video refresh address high byte")	\
	sh( 3, vcountmid, VCOUNTMID, "Video refresh address mid byte")	\
	sh( 4, vcountlo,  VCOUNTLO,  "Video refresh address low byte")	\
	sh( 5, synch_m,   SYNC_M,    "Synchronisation mode")		\
	sh( 6, vbaselo,   VBASELO,   "Video display base low byte")	\
	sh( 7, linewid,   LINEWID,   "Extra words per scanline")	\
	sh(32, color_00,  COLOR_00,  "Palette color 0")			\
	sh(33, color_01,  COLOR_01,  "Palette color 1")			\
	sh(34, color_02,  COLOR_02,  "Palette color 2")			\
	sh(35, color_03,  COLOR_03,  "Palette color 3")			\
	sh(36, color_04,  COLOR_04,  "Palette color 4")			\
	sh(37, color_05,  COLOR_05,  "Palette color 5")			\
	sh(38, color_06,  COLOR_06,  "Palette color 6")			\
	sh(39, color_07,  COLOR_07,  "Palette color 7")			\
	sh(40, color_08,  COLOR_08,  "Palette color 8")			\
	sh(41, color_09,  COLOR_09,  "Palette color 9")			\
	sh(42, color_10,  COLOR_10,  "Palette color 10")		\
	sh(43, color_11,  COLOR_11,  "Palette color 11")		\
	sh(44, color_12,  COLOR_12,  "Palette color 12")		\
	sh(45, color_13,  COLOR_13,  "Palette color 13")		\
	sh(46, color_14,  COLOR_14,  "Palette color 14")		\
	sh(47, color_15,  COLOR_15,  "Palette color 15")		\
	sh(48, video_m,   VIDEO_M,   "Video mode resoluation")		\
	sh(50, hscroll,   HSCROLL,   "Horizontal scroll")

extern const struct device shifter_device;

#endif /* ATARI_SHIFTER_H */
