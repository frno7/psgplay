// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#ifndef ATARI_MFP_MAP_H
#define ATARI_MFP_MAP_H

#include "mfp-register.h"

#define MFP_BUS_ADDRESS 0xfffa00

struct mfp_map {
#define MFP_REG_MAP(register_, symbol_, label_, description_)		\
	u8 : 8;								\
	__volatile__ struct cf68901_##symbol_ symbol_;
CF68901_REGISTERS(MFP_REG_MAP)
};

static inline struct mfp_map *mfp_map(void)
{
	return (struct mfp_map *)MFP_BUS_ADDRESS;
}

#endif /* ATARI_MFP_MAP_H */
