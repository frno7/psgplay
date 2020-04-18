// SPDX-License-Identifier: GPL-2.0

#ifndef ATARI_EXCEPTION_VECTOR_H
#define ATARI_EXCEPTION_VECTOR_H

#include "psgplay/types.h"

#define EXCEPTION_VECTOR(e)						\
	e(  0, "Reset initial interrupt stack pointer")			\
	e(  1, "Reset initial program counter")				\
	e(  2, "Access fault")						\
	e(  3, "Address error")						\
	e(  4, "Illegal instruction")					\
	e(  5, "Integer divide by zero")				\
	e(  6, "CHK instruction")					\
	e(  7, "TRAPV instruction")					\
	e(  8, "Privilege violation")					\
	e(  9, "Trace")							\
	e( 10, "Line 1010 emulator")					\
	e( 11, "Line 1111 emulator")					\
	e( 12, "Reserved")						\
	e( 13, "Reserved")						\
	e( 14, "Reserved")						\
	e( 15, "Uninitialized interrupt")				\
	e( 16, "Reserved")						\
	e( 17, "Reserved")						\
	e( 18, "Reserved")						\
	e( 19, "Reserved")						\
	e( 20, "Reserved")						\
	e( 21, "Reserved")						\
	e( 22, "Reserved")						\
	e( 23, "Reserved")						\
	e( 24, "Spurious interrupt")					\
	e( 25, "Interrupt level 1")					\
	e( 26, "Interrupt level 2 HBL")					\
	e( 27, "Interrupt level 3")					\
	e( 28, "Interrupt level 4 VBL")					\
	e( 29, "Interrupt level 5")					\
	e( 30, "Interrupt level 6 MFP")					\
	e( 31, "Interrupt level 7")					\
	e( 32, "Trap #0")						\
	e( 33, "Trap #1 GEMDOS")					\
	e( 34, "Trap #2 GEM")						\
	e( 35, "Trap #3")						\
	e( 36, "Trap #4")						\
	e( 37, "Trap #5")						\
	e( 38, "Trap #6")						\
	e( 39, "Trap #7")						\
	e( 40, "Trap #8")						\
	e( 41, "Trap #9")						\
	e( 42, "Trap #10")						\
	e( 43, "Trap #11")						\
	e( 44, "Trap #12")						\
	e( 45, "Trap #13 BIOS")						\
	e( 46, "Trap #14 XBIOS")					\
	e( 47, "Trap #15")						\
	e( 48, "Reserved")						\
	e( 49, "Reserved")						\
	e( 50, "Reserved")						\
	e( 51, "Reserved")						\
	e( 52, "Reserved")						\
	e( 53, "Reserved")						\
	e( 54, "Reserved")						\
	e( 55, "Reserved")						\
	e( 56, "Reserved")						\
	e( 57, "Reserved")						\
	e( 58, "Reserved")						\
	e( 59, "Reserved")						\
	e( 60, "Reserved")						\
	e( 61, "Reserved")						\
	e( 62, "Reserved")						\
	e( 63, "Reserved")						\
	e( 64, "Centronics busy")					\
	e( 65, "RS-232 carrier detect")					\
	e( 66, "RS-232 clear to send")					\
	e( 67, "Blitter done")						\
	e( 68, "MFP timer D")						\
	e( 69, "MFP timer C")						\
	e( 70, "ACIA keyboard processor")				\
	e( 71, "Polled FDC/HDC")					\
	e( 72, "MFP timer B")						\
	e( 73, "RS-232 transmit error")					\
	e( 74, "RS-232 transmit buffer empty")				\
	e( 75, "RS-232 receive error")					\
	e( 76, "RS-232 receive buffer full")				\
	e( 77, "MFP timer A")						\
	e( 78, "RS-232 ring indicator")					\
	e( 79, "Monochrome monitor detect")				\
	e( 80, "Reserved")						\
	e( 81, "Reserved")						\
	e( 82, "Reserved")						\
	e( 83, "Reserved")						\
	e( 84, "Reserved")						\
	e( 85, "Reserved")						\
	e( 86, "Reserved")						\
	e( 87, "Reserved")						\
	e( 88, "Reserved")						\
	e( 89, "Reserved")						\
	e( 90, "Reserved")						\
	e( 91, "Reserved")						\
	e( 92, "Reserved")						\
	e( 93, "Reserved")						\
	e( 94, "Reserved")						\
	e( 95, "Reserved")						\
	e( 96, "Reserved")						\
	e( 97, "Reserved")						\
	e( 98, "Reserved")						\
	e( 99, "Reserved")						\
	e(100, "Reserved")						\
	e(101, "Reserved")						\
	e(102, "Reserved")						\
	e(103, "Reserved")						\
	e(104, "Reserved")						\
	e(105, "Reserved")						\
	e(106, "Reserved")						\
	e(107, "Reserved")						\
	e(108, "Reserved")						\
	e(109, "Reserved")						\
	e(110, "Reserved")						\
	e(111, "Reserved")						\
	e(112, "Reserved")						\
	e(113, "Reserved")						\
	e(114, "Reserved")						\
	e(115, "Reserved")						\
	e(116, "Reserved")						\
	e(117, "Reserved")						\
	e(118, "Reserved")						\
	e(119, "Reserved")						\
	e(120, "Reserved")						\
	e(121, "Reserved")						\
	e(122, "Reserved")						\
	e(123, "Reserved")						\
	e(124, "Reserved")						\
	e(125, "Reserved")						\
	e(126, "Reserved")						\
	e(127, "Reserved")						\

const char *exception_vector_description(u32 address);

#endif /* ATARI_EXCEPTION_VECTOR_H */
