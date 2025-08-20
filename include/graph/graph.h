// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2025 Fredrik Noring
 */

#ifndef PSGPLAY_GRAPH_H
#define PSGPLAY_GRAPH_H

#include "internal/types.h"

#include "audio/audio.h"

struct graph_bounds {
	double min_x;
	double min_y;
	double max_x;
	double max_y;
};

struct graph_encoder_cb {
	/**
	 * f - callback with successively encoded data
	 *
	 * @param data encoded data
	 * @param size size of encoded data
	 * @param arg argument pointer
	 * @return true to continue processing, otherwise false
	 */
	bool (*f)(const void *data, size_t size, void *arg);
	void *arg;
};

struct graph_encoder {
	struct graph_bounds bounds;
	struct graph_encoder_cb cb;
	const struct graph_encoder_module *module;
};

struct graph_encoder_module {
	bool (*header)(const struct graph_encoder * const encoder);
	bool (*footer)(const struct graph_encoder * const encoder);
	bool (*axes)(const struct graph_encoder * const encoder);
	bool (*samples)(const struct graph_encoder * const encoder,
		struct audio *audio);
	bool (*square_wave)(const struct graph_encoder * const encoder,
		struct audio_wave wave, int16_t minimum, int16_t maximum);
};

struct graph_encoder *graph_encoder_init(struct graph_bounds bounds,
	struct graph_encoder_cb cb, const struct graph_encoder_module *module);

void graph_encoder_free(struct graph_encoder * const encoder);

#endif /* PSGPLAY_GRAPH_H */
