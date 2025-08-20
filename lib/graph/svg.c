// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2025 Fredrik Noring
 */

#include <stdio.h>
#include <string.h>

#include "internal/macro.h"

#include "audio/audio.h"
#include "graph/svg.h"

static bool svg_encode_header(const struct graph_encoder * const encoder)
{
	const double w = encoder->bounds.max_x - encoder->bounds.min_x;
	const double h = encoder->bounds.max_y - encoder->bounds.min_y;
	const double x = encoder->bounds.min_x;
	const double y = encoder->bounds.min_y;
	char header[1024];

	snprintf(header, sizeof(header),
		"<?xml version=\"1.0\"?>\n"
		"<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.1//EN\"\n"
		"  \"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd\">\n"
		"<svg version=\"1.1\"\n"
		"     viewBox=\"%.1f %.1f %.1f %.1f\" "
		"xmlns=\"http://www.w3.org/2000/svg\">\n",
		x, y,
		w, h);

	return encoder->cb.f(header, strlen(header), encoder->cb.arg);
}

static bool svg_encode_footer(const struct graph_encoder * const encoder)
{
	static const char * const footer = "</svg>\n";

	return encoder->cb.f(footer, strlen(footer), encoder->cb.arg);
}

static bool svg_encode_axes(const struct graph_encoder * const encoder)
{
	char axis[1024];

	snprintf(axis, sizeof(axis),
		"  <polyline fill=\"none\" stroke=\"#808080\" "
		"stroke-width=\"%f\" points=\"%f,%f %f,%f\"/>\n",
		0.2,
		encoder->bounds.min_x,
		(encoder->bounds.max_y - encoder->bounds.min_y) / 2,
		encoder->bounds.max_x,
		(encoder->bounds.max_y - encoder->bounds.min_y) / 2);

	return encoder->cb.f(axis, strlen(axis), encoder->cb.arg);
}

static bool svg_encode_samples(const struct graph_encoder * const encoder,
	struct audio *audio)
{
	const char *marker =
		"  <defs>\n"
		"    <marker\n"
		"      id=\"sample-box\"\n"
		"      viewBox=\"0 0 4 4\"\n"
		"      refX=\"2\"\n"
		"      refY=\"2\"\n"
		"      markerWidth=\"4\"\n"
		"      markerHeight=\"4\"\n"
		"      orient=\"auto-start-reverse\">\n"
		"      <path d=\"M 0 0 L 0 4 L 4 4 L 4 0 z\"/>\n"
		"    </marker>\n"
		"  </defs>\n";

	if (!encoder->cb.f(marker, strlen(marker), encoder->cb.arg))
		return false;

	for (size_t i = 0; i < audio->format.sample_count; i += 1) {
		const double x = i * 4.0;
		const double y = (100 * audio->samples[i].right) / 32768.0;
		char bar[1024];

		snprintf(bar, sizeof(bar),
			"  <line stroke=\"#000000\" "
			"stroke-width=\"%f\" marker-end=\"url(#sample-box)\" "
			"x1=\"%f\" y1=\"%f\" x2=\"%f\" y2=\"%f\"/>\n",
			0.5,
			x, (encoder->bounds.max_y - encoder->bounds.min_y) / 2,
			x, (encoder->bounds.max_y - encoder->bounds.min_y) / 2 - y);

		if (!encoder->cb.f(bar, strlen(bar), encoder->cb.arg))
			return false;
	}

	return true;
}

static bool svg_encode_square_wave(
	const struct graph_encoder * const encoder,
	struct audio_wave wave, int16_t minimum, int16_t maximum)
{
	char buffer[1024];

	snprintf(buffer, sizeof(buffer),
		"  <polyline fill=\"none\" stroke=\"#ff0000\" "
		"stroke-width=\"%f\" points=\"", 1.0);

	if (!encoder->cb.f(buffer, strlen(buffer), encoder->cb.arg))
		return false;

	const struct {
		double x;
		double y;
	} points[6] = {
		{ .x = wave.phase,                     .y =       0 },
		{ .x = wave.phase,                     .y = maximum },
		{ .x = wave.phase + 0.5 * wave.period, .y = maximum },
		{ .x = wave.phase + 0.5 * wave.period, .y = minimum },
		{ .x = wave.phase +       wave.period, .y = minimum },
		{ .x = wave.phase +       wave.period, .y =       0 },
	};

	for (ssize_t k = -1; k < 3; k++)
	for (ssize_t i = 0; i < ARRAY_SIZE(points); i++) {
		const double x = (k * wave.period + points[i].x) * 4.0;
		const double y = (100 * points[i].y) / 32768.0;

		snprintf(buffer, sizeof(buffer), "%f,%f ",
			x, (encoder->bounds.max_y - encoder->bounds.min_y) / 2 - y);

		if (!encoder->cb.f(buffer, strlen(buffer), encoder->cb.arg))
			return false;
	}

	snprintf(buffer, sizeof(buffer), "\"/>");

	return encoder->cb.f(buffer, strlen(buffer), encoder->cb.arg);
}

const struct graph_encoder_module svg_encoder = {
	.header      = svg_encode_header,
	.footer      = svg_encode_footer,
	.axes        = svg_encode_axes,
	.samples     = svg_encode_samples,
	.square_wave = svg_encode_square_wave,
};
