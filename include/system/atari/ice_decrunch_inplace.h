// SPDX-License-Identifier: GPL-2.0

#ifndef PSGPLAY_SYSTEM_ATARI_ICE_H
#define PSGPLAY_SYSTEM_ATARI_ICE_H

#include <stdbool.h>
#include <stddef.h>

#include "internal/types.h"

struct ice_decrunch_inplace {
	u32 d[8];
	void *a[7];
	u32 d0;
	u8 save[120];
	size_t size;
};

struct ice_decrunch_inplace ice_decrunch_inplace_init(
	const void *data, size_t size);

bool ice_decrunch_inplace_partial(void *data,
	struct ice_decrunch_inplace *context, size_t *offset);

void ice_decrunch_inplace_all(void *data, size_t size);

#endif /* PSGPLAY_SYSTEM_ATARI_ICE_H */
