// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#include "psgplay/ice.h"

#include "system/atari/ice_decrunch_inplace.h"

bool ice_decrunch_inplace_(void *data, struct ice_decrunch_inplace *context);

struct ice_decrunch_inplace ice_decrunch_inplace_init(
	const void *data, size_t size)
{
	return (struct ice_decrunch_inplace) {
		.size = ice_decrunched_size(data, size)
	};
}

bool ice_decrunch_inplace_partial(void *data,
	struct ice_decrunch_inplace *context, size_t *offset)
{
	const bool more = ice_decrunch_inplace_(data, context);
	const size_t n = more ? context->a[6] - context->a[4] : 0;

	if (offset)
		*offset = context->size - n;

	return more;
}

void ice_decrunch_inplace_all(void *data, size_t size)
{
	struct ice_decrunch_inplace context =
		ice_decrunch_inplace_init(data, size);

	do { } while (ice_decrunch_inplace_partial(data, &context, NULL));
}
