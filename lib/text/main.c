// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "psgplay/sndh.h"

#include "text/main.h"
#include "text/view.h"

static void main_title(struct vt_buffer *vtb, int row, const char *title)
{
	for (int c = 0, i = 0; c < vtb->server.size.cols; c++)
		vt_putc_reverse(vtb, row, c, title[i] ? title[i++] : ' ');
}

static void main_form(struct vt_buffer *vtb, struct text_model *model)
{
	for (int i = 0; i < model->subtune_count; i++)
		vt_printf(vtb, 4 + i, 1, vt_attr_normal, "%2d.", i + 1);
}

static void main_data(struct vt_buffer *vtb, struct text_model *model)
{
	int subtune = 0;

	sndh_for_each_tag (model->sndh.data, model->sndh.size)
		if (strcmp(sndh_tag_name, "COMM") == 0) {
			vt_printf(vtb, 1, 0, vt_attr_normal,
				"%s", sndh_tag_value);
		} else if (strcmp(sndh_tag_name, "!#SN") == 0) {
			subtune++;

			vt_printf(vtb, 3 + subtune, 5,
				vt_attr_normal, "%s", sndh_tag_value);
		}

	if (!subtune)
		for (int i = 0; i < model->subtune_count; i++)
			vt_printf(vtb, 4 + i, 5, vt_attr_normal,
				"%s", model->title);

	vt_printf(vtb, 2, 0, vt_attr_normal, "%s", model->title);
}

static void cursor_hide(struct vt_buffer *vtb, struct text_model *model)
{
	vt_putc_normal(vtb, 3 + model->cursor.index, 0, ' ');
}

static void cursor_show(struct vt_buffer *vtb, struct text_model *model)
{
	vt_putc_normal(vtb, 3 + model->cursor.index, 0, '>');
}

static void main_init(struct vt_buffer *vtb, struct text_model *model)
{
	vt_clear(vtb);

	if (!model->cursor.index)
		model->cursor.index = model->ctrl.track;

	main_title(vtb, 0, "PSG play");

	main_form(vtb, model);
	main_data(vtb, model);

	main_title(vtb, vtb->server.size.rows - 1, "");

	cursor_show(vtb, model);
}

static bool main_event(struct vt_buffer *vtb, struct fifo_utf32 *in_utf32,
	struct text_model *model)
{
	unicode_t key;

	for_each_fifo_utf32 (key, in_utf32) {

		if ((key == 'j' || key == U_ARROW_DOWN) &&
				model->cursor.index < model->subtune_count) {
			cursor_hide(vtb, model);
			model->cursor.index++;
			cursor_show(vtb, model);
		}

		if ((key == 'k' || key == U_ARROW_UP) &&
				model->cursor.index > 1) {
			cursor_hide(vtb, model);
			model->cursor.index--;
			cursor_show(vtb, model);
		}

		if (key == 'q' || key == '\033')
			return false;
	}

	return true;
}

const struct text_view text_view_main = {
	.init = main_init,
	.event = main_event
};

