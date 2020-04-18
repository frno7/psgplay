// SPDX-License-Identifier: GPL-2.0

#ifndef PSGPLAY_MEMORY_H
#define PSGPLAY_MEMORY_H

#include "psgplay/types.h"

void *xmalloc(size_t size);

void *zalloc(size_t size);

void *xrealloc(void *ptr, size_t size);

void *xmemdup(const void *ptr, size_t size);

#endif /* PSGPLAY_MEMORY_H */
