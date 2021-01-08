// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2020 Fredrik Noring
 */

#ifndef ATARI_MMU_H
#define ATARI_MMU_H

#include "internal/types.h"

u8 dma_read_memory_8(u32 bus_address);

u16 dma_read_memory_16(u32 bus_address);

u8 probe_read_memory_8(u32 bus_address);

u16 probe_read_memory_16(u32 bus_address);

void probe_copy_memory_8(void *buffer, u32 bus_address, size_t byte_count);

void probe_copy_memory_16(void *buffer, u32 bus_address, size_t word_count);

#endif /* ATARI_MMU_H */
