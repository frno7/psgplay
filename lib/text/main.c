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
#include "text/mode.h"

static void main_title(struct vt_buffer *vtb, int row, const char *title)
{
	for (int c = 0, i = 0; c < vtb->server.size.cols; c++)
		vt_putc_reverse(vtb, row, c, title[i] ? title[i++] : ' ');
}

static void main_form(struct vt_buffer *vtb, const struct text_sndh *sndh)
{
	for (int i = 0; i < sndh->subtune_count; i++)
		vt_printf(vtb, 4 + i, 1, vt_attr_normal, "%2d.", i + 1);
}

static void main_data(struct vt_buffer *vtb, const struct text_sndh *sndh)
{
	int subtune = 0;

	sndh_for_each_tag (sndh->data, sndh->size)
		if (strcmp(sndh_tag_name, "COMM") == 0) {
			vt_printf(vtb, 1, 0, vt_attr_normal,
				"%s", sndh_tag_value);
		} else if (strcmp(sndh_tag_name, "!#SN") == 0) {
			subtune++;

			vt_printf(vtb, 3 + subtune, 5,
				vt_attr_normal, "%s", sndh_tag_value);
		}

	if (!subtune)
		for (int i = 0; i < sndh->subtune_count; i++)
			vt_printf(vtb, 4 + i, 5, vt_attr_normal,
				"%s", sndh->title);

	vt_printf(vtb, 2, 0, vt_attr_normal, "%s", sndh->title);
}

static void cursor_hide(struct vt_buffer *vtb, int cursor)
{
	vt_putc_normal(vtb, 3 + cursor, 0, ' ');
}

static void cursor_show(struct vt_buffer *vtb, int cursor)
{
	vt_putc_normal(vtb, 3 + cursor, 0, '>');
}

static void main_view(struct vt_buffer *vtb, struct text_state *view,
	const struct text_state *model, const struct text_sndh *sndh)
{
	if (!view->cursor) {
		*view = *model;

		vt_clear(vtb);

		main_title(vtb, 0, "PSG play");

		main_form(vtb, sndh);
		main_data(vtb, sndh);

		main_title(vtb, vtb->server.size.rows - 1, "");

		cursor_show(vtb, view->cursor);
	} else {
		cursor_hide(vtb, view->cursor);

		*view = *model;

		cursor_show(vtb, view->cursor);
	}
}

static void main_ctrl(const unicode_t key, struct text_state *ctrl,
	const struct text_state *model, const struct text_sndh *sndh)
{
	if (key == 'j' || key == U_ARROW_DOWN)
		ctrl->cursor++;

	if (key == 'k' || key == U_ARROW_UP)
		ctrl->cursor--;

	ctrl->quit = (key == 'q' || key == '\033');
}

const struct text_mode text_mode_main = {
	.view = main_view,
	.ctrl = main_ctrl,
};
