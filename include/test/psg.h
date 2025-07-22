// SPDX-License-Identifier: GPL-2.0

#ifndef TEST_PSG_H
#define TEST_PSG_H

#include "cf2149/module/cf2149.h"

#include "atari/psg.h"

#include "internal/macro.h"
#include "internal/types.h"

#include "test/io.h"

#define PSG_REG_RD_SELECT	0xff8800
#define PSG_REG_WR		0xff8802

static inline uint8_t psg_rd(uint8_t reg)
{
	iowr8(reg, PSG_REG_RD_SELECT);
	return iord8(PSG_REG_RD_SELECT);
}

static inline void psg_wr(uint8_t value, uint8_t reg)
{
	iowr8(reg,   PSG_REG_RD_SELECT);
	iowr8(value, PSG_REG_WR);
}

static inline void psg_wr_period_a(uint16_t value)
{
	psg_wr(value & 0xff, CF2149_REG_PLO_A);
	psg_wr(value >> 8, CF2149_REG_PHI_A);
}

static inline void psg_wr_level_a(uint8_t value)
{
	psg_wr(value, CF2149_REG_LEVEL_A);
}

static inline void psg_wr_iomix(uint8_t value)
{
	psg_wr(value, CF2149_REG_IOMIX);
}

#endif /* TEST_PSG_H */
