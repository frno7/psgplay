// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#ifndef ATARI_MMU_TRACE_H
#define ATARI_MMU_TRACE_H

#include "internal/types.h"

#include "atari/device.h"

void mmu_trace_rd_u8(u32 dev_address, u32 value, const struct device *bd);
void mmu_trace_rd_u16(u32 dev_address, u32 value, const struct device *bd);
void mmu_trace_wr_u8(u32 dev_address, u32 value, const struct device *bd);
void mmu_trace_wr_u16(u32 dev_address, u32 value, const struct device *bd);

#endif /* ATARI_MMU_TRACE_H */
