// SPDX-License-Identifier: GPL-2.0
/* Copyright (C) 2025 Fredrik Noring */

#ifndef PSGPLAY_TEST_IO_H
#define PSGPLAY_TEST_IO_H

#include "internal/types.h"

#define iobarrier_rw()	__sync_synchronize()

#define DEFINE_IORD(type, name)						\
static inline type name(const u32 addr)					\
{									\
	const volatile type *__addr = (const volatile type *)addr;	\
									\
	iobarrier_rw();							\
									\
	return *__addr;							\
}

DEFINE_IORD(uint8_t,  iord8);
DEFINE_IORD(uint16_t, iord16);
DEFINE_IORD(uint32_t, iord32);

#define DEFINE_IOWR(type, name)						\
static inline void name(type value, u32 addr)				\
{									\
	volatile type *__addr = (volatile type *)addr;			\
									\
	iobarrier_rw();							\
									\
	*__addr = value;						\
}

DEFINE_IOWR(uint8_t,  iowr8);
DEFINE_IOWR(uint16_t, iowr16);
DEFINE_IOWR(uint32_t, iowr32);

#endif /* PSGPLAY_TEST_IO_H */
