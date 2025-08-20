// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2025 Fredrik Noring
 */

#include <stdlib.h>

#include "system/unix/memory.h"

#include "graph/graph.h"

struct graph_encoder *graph_encoder_init(struct graph_bounds bounds,
	struct graph_encoder_cb cb, const struct graph_encoder_module *module)
{
	struct graph_encoder *encoder = zalloc(sizeof(*encoder));

	encoder->bounds = bounds;
	encoder->cb = cb;
	encoder->module = module;

	return encoder;
}

void graph_encoder_free(struct graph_encoder * const encoder)
{
	free(encoder);
}
