// SPDX-License-Identifier: GPL-2.0

#ifndef PSGPLAY_SYSTEM_UNIX_MEMORY_H
#define PSGPLAY_SYSTEM_UNIX_MEMORY_H

#include <stddef.h>

void *xmalloc(size_t size);

void *zalloc(size_t size);

void *xrealloc(void *ptr, size_t size);

void *xmemdup(const void *ptr, size_t size);

#endif /* PSGPLAY_SYSTEM_UNIX_MEMORY_H */
