// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "psgplay/sndh.h"

#include "text/load.h"
#include "text/mode.h"

static const u32 logo[] = {
	0b00000111111011111110011111100000,
	0b00000110011011000000011001100000,
	0b00000110011011111110011000000000,
	0b00000111111000000110111011100000,
	0b00001110000011100110111001100000,
	0b00001110000011100110111001100000,
	0b00001110000011111110111111100000,
	0b00000000000000000000000000000000,
	0b00000000001100000000000000000000,
	0b00000000001100000000000000000000,
	0b00111111101100111111101100011000,
	0b00110001101100000001101110011000,
	0b00110001101110111111101110011000,
	0b00111111101110111001101111111000,
	0b00111000001110111111100000011000,
	0b00111000000000000000001111111000,
};

static void vt_printf_centre(struct vt_buffer *vtb, int row,
   struct vt_attr attr, const char *fmt, ...)
{
	char msg[64];
	va_list ap;

	va_start(ap, fmt);

	vsnprintf(msg, sizeof(msg), fmt, ap);

	const int offset = (39 - strlen(msg)) / 2;

	for (int i = 0; msg[i]; i++)
		vt_putc(vtb, row, offset + i, msg[i], attr);

	va_end(ap);
}

static void load_bar(struct vt_buffer *vtb, struct text_state *view,
	const struct text_state *model, const struct text_sndh *sndh,
	u64 timestamp)
{
	const size_t progress = (27 * model->progress) / 100;

	for (size_t col = 0; col < 27; col++)
		vt_putc(vtb, ARRAY_SIZE(logo) + 4, 6 + col, '-',
			col + 1 <= progress ? vt_attr_reverse : vt_attr_normal);
}

static void load_init(struct vt_buffer *vtb, struct text_state *view,
	const struct text_state *model, const struct text_sndh *sndh,
	u64 timestamp)
{
	vt_clear(vtb);

	for (size_t row = 0; row < ARRAY_SIZE(logo); row++)
		for (size_t col = 0; col < 32; col++)
			if (logo[row] & (0x80000000 >> col))
				vt_putc_reverse(vtb, row, 4 + col, ' ');

	vt_printf_centre(vtb, ARRAY_SIZE(logo) + 2,
		vt_attr_normal, "Loading %s", sndh->title);

	load_bar(vtb, view, model, sndh, timestamp);
}

static u64 load_view(struct vt_buffer *vtb, struct text_state *view,
	const struct text_state *model, const struct text_sndh *sndh,
	u64 timestamp)
{
	if (view->mode != model->mode) {
		view->mode = model->mode;
		view->progress = 0;

		load_init(vtb, view, model, sndh, timestamp);
	}

	if (view->progress != model->progress) {
		load_bar(vtb, view, model, sndh, timestamp);

		view->progress = model->progress;
	}

	return 0;
}

static void load_ctrl(const unicode_t key, struct text_state *ctrl,
	const struct text_state *model, const struct text_sndh *sndh)
{
	ctrl->redraw = false;

	switch (key) {
	case 0:
		break;
	case '\014':	/* ^L */
		ctrl->redraw = true;
		break;
	case 'q':
	case '\033':	/* Escape */
		ctrl->quit = true;
		break;
	}
}

const struct text_mode text_mode_load = {
	.view = load_view,
	.ctrl = load_ctrl,
};
