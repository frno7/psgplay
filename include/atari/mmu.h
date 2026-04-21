// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2020 Fredrik Noring
 */

#ifndef ATARI_MMU_H
#define ATARI_MMU_H

#include "internal/types.h"

uint8_t dma_read_memory_8(struct machine *machine, uint32_t bus_address);

uint16_t dma_read_memory_16(struct machine *machine, uint32_t bus_address);

uint8_t probe_read_memory_8(struct machine *machine, uint32_t bus_address);

uint16_t probe_read_memory_16(struct machine *machine, uint32_t bus_address);

void probe_copy_memory_8(struct machine *machine,
	void *buffer, uint32_t bus_address, size_t byte_count);

void probe_copy_memory_16(struct machine *machine,
	void *buffer, uint32_t bus_address, size_t word_count);

#endif /* ATARI_MMU_H */
