// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "psgplay/assert.h"	/* FIXME: psgplay -> internal */
#include "internal/build-assert.h"
#include "internal/types.h"

#include "vt/vt.h"

#define VT_ESCAPE_TIME	10	/* Time in ms */

void vt_putc(struct vt_buffer *vtb, int row, int col,
	vt_char c, struct vt_attr attr)
{
	if (row < 0 || vtb->server.size.rows <= row ||
	    col < 0 || vtb->server.size.cols <= col)
		return;

	const int offset = row * vtb->server.size.cols + col;
	const vt_char u = isspace(c) ? '\0' : c;

	if (vtb->server.chars[offset] == u &&
	    vtb->server.attrs[offset].state == attr.state)
		return;

	vtb->server.chars[offset] = u;
	vtb->server.attrs[offset].state = attr.state;

	if (vtb->client.chars[offset] == vtb->server.chars[offset] &&
	    vtb->client.attrs[offset].state == vtb->server.attrs[offset].state)
		return;

	/* FIXME: Mark as dirty */
}

void vt_putc_normal(struct vt_buffer *vtb, int row, int col, vt_char c)
{
	vt_putc(vtb, row, col, c, (struct vt_attr) { .reverse = false });
}

void vt_putc_reverse(struct vt_buffer *vtb, int row, int col, vt_char c)
{
	vt_putc(vtb, row, col, c, (struct vt_attr) { .reverse = true });
}

int vt_vprintf(struct vt_buffer *vtb, int row, int col,
	struct vt_attr attr, const char *fmt, va_list ap)
{
	char msg[256];
	size_t i;

	vsnprintf(msg, sizeof(msg), fmt, ap);

	for (i = 0; msg[i]; i++)
		if (msg[i] == '\n') {
			row++;
			col = 0;
		} else
			vt_putc(vtb, row, col++, msg[i], attr);

	return i;
}

int vt_printf(struct vt_buffer *vtb, int row, int col,
   struct vt_attr attr, const char *fmt, ...)
{
	va_list ap;
	int r;

	va_start(ap, fmt);
	r = vt_vprintf(vtb, row, col, attr, fmt, ap);
	va_end(ap);

	return r;
}

static inline void vt_output_append(struct vt_buffer *vtb, struct vt_text txt)
{
	const char *s = vt_text(txt);
	const size_t length = strlen(s);

	if (vtb->output.index == vtb->output.size)
		vtb->output.index = vtb->output.size = 0;

	BUG_ON(vtb->output.size + length > sizeof(vtb->output.chars));

	memcpy(&vtb->output.chars[vtb->output.size], s, length);
	vtb->output.size += length;
}

static inline void vt_output_append_char(struct vt_buffer *vtb, vt_char c)
{
	BUG_ON(sizeof(vt_char) != 1);

	if (vtb->output.index == vtb->output.size)
		vtb->output.index = vtb->output.size = 0;

	BUG_ON(vtb->output.size + 1 > sizeof(vtb->output.chars));

	vtb->output.chars[vtb->output.size] = c;
	vtb->output.size++;
}

static u8 vt_getc_reset(struct vt_buffer *vtb)
{
	vtb->output.redraw = false;

	vt_output_append(vtb, vtb->cmd->hide());	/* FIXME */

	vt_output_append(vtb, vtb->cmd->clear());
	vt_output_append(vtb, vtb->cmd->position(0, 0));
	vt_output_append(vtb, vtb->cmd->reset());

	vtb->cursor.row = 0;
	vtb->cursor.col = 0;
	vtb->cursor.attr.state = 0;

	vtb->output.offset = 0;
	vtb->output.row = 0;
	vtb->output.col = 0;

	const size_t area = vtb->server.size.rows * vtb->server.size.cols;
	memset(vtb->client.chars, 0, sizeof(vt_char) * area);
	memset(vtb->client.attrs, 0, sizeof(struct vt_attr) * area);

	return vt_getc(vtb);
}

static u8 vt_getc_clear(struct vt_buffer *vtb)
{
	vtb->clear = false;

	return vt_getc_reset(vtb);
}

static inline void vt_getc_advance_output(struct vt_buffer *vtb)
{
	vtb->output.offset++;

	if (vtb->output.col + 1 < vtb->server.size.cols) {
		vtb->output.col++;
		return;
	}

	vtb->output.col = 0;

	if (vtb->output.row + 1 < vtb->server.size.rows) {
		vtb->output.row++;
		return;
	}

	vtb->output.row = 0;
	vtb->output.offset = 0;
}

static inline vt_char vt_getc_output(struct vt_buffer *vtb)
{
	const int row = vtb->output.row +
		(vtb->client.size.rows - vtb->server.size.rows) / 2;
	const int col = vtb->output.col +
		(vtb->client.size.cols - vtb->server.size.cols) / 2;
	const int offset = vtb->output.offset;
	const vt_char c = vtb->server.chars[offset] ?
		vtb->server.chars[offset] : ' ';
	const struct vt_attr a = vtb->server.attrs[offset];

	if (vtb->client.chars[offset] == c &&
	    vtb->client.attrs[offset].state == a.state)
		return 0;

	if (row < 0 || vtb->client.size.rows <= row ||
	    col < 0 || vtb->client.size.cols <= col)
		return 0;

	if (row != vtb->cursor.row ||
	    col != vtb->cursor.col)
		vt_output_append(vtb, vtb->cmd->position(row, col));

	if (vtb->cursor.attr.reverse != a.reverse) {
		vt_output_append(vtb, (a.reverse ?
			vtb->cmd->reverse :
			vtb->cmd->normal)());

		vtb->cursor.attr.state = a.state;
	}

	vt_output_append_char(vtb, c);

	vtb->client.chars[offset] = c;
	vtb->client.attrs[offset].state = a.state;
	vtb->cursor.col++;

	return vt_getc(vtb);
}

u8 vt_getc(struct vt_buffer *vtb)
{
	if (vtb->output.index < vtb->output.size)
		return vtb->output.chars[vtb->output.index++];

	if (vtb->clear)
		return vt_getc_clear(vtb);

	if (vtb->output.redraw)
		return vt_getc_reset(vtb);

	for (int row = 0; row < vtb->server.size.rows; row++)
	for (int col = 0; col < vtb->server.size.cols; col++) {
		const vt_char c = vt_getc_output(vtb);

		vt_getc_advance_output(vtb);

		if (c)
			return c;
	}

	return 0;	/* Nothing to do */
}

ssize_t vt_read(struct vt_buffer *vtb, void *data, size_t count)
{
	u8 *b = data;

	for (size_t i = 0; i < count; i++) {
		const u8 c = vt_getc(vtb);

		if (!c)
			return i;

		b[i] = c;
	}

	return count;
}

ssize_t vt_write_fifo(struct vt_buffer *vtb, struct fifo *f)
{
	u8 buffer[256];

	const size_t r = vt_read(vtb, buffer,
		min(sizeof(buffer), fifo_remaining(f)));
	const size_t w = fifo_write(f, buffer, r);

	BUG_ON(w != r);

	return w;
}

u64 vt_event(struct vt_buffer *vtb, u64 timestamp)
{
	if (!vtb->input.escape)
		return 0;

	if (timestamp - vtb->input.timestamp < VT_ESCAPE_TIME)
		return vtb->input.timestamp + VT_ESCAPE_TIME;

	if (vtb->input.size == 1) {
		vtb->input.drain = true;	/* Output escape */
		vtb->input.escape = false;
	}

	return 0;
}

static char vt_deescape_char(struct vt_buffer *vtb, char c, u64 timestamp)
{
	if (!vtb->input.escape) {
		vtb->input.escape = (c == '\033');

		if (!vtb->input.escape)
			return c;

		BUG_ON(vtb->input.drain);

		vtb->input.timestamp = timestamp;
		vtb->input.size = 1;
		vtb->input.index = 0;
		vtb->input.buffer[0] = c;
		vtb->input.buffer[1] = '\0';

		vt_event(vtb, timestamp);

		return 0;
	}

	if (timestamp - vtb->input.timestamp >= VT_ESCAPE_TIME) {
		vtb->input.size = 0;	/* Discard invalid sequence */
		vtb->input.escape = false;

		return vt_deescape_char(vtb, c, timestamp);
	}

	if (vtb->input.size + 1 >= sizeof(vtb->input.buffer))
		return 0;

	vtb->input.buffer[vtb->input.size++] = c;
	vtb->input.buffer[vtb->input.size] = '\0';

	if (!vtb->cmd || !vtb->cmd->deescape)
		return 0;

	const struct vt_text t = vtb->cmd->deescape(vtb->input.buffer);
	const char *s = vt_text(t);

	if (!s[0])
		return 0;

	const size_t length = strlen(s);

	BUG_ON(length + 1 >= sizeof(vtb->input.buffer));
	memcpy(vtb->input.buffer, s, length + 1);
	vtb->input.size = length;
	vtb->input.drain = true;

	return 0;
}

ssize_t vt_deescape_fifo(struct vt_buffer *vtb,
	struct fifo *dst, struct fifo *src, u64 timestamp)
{
	size_t t = 0;

	while (!fifo_full(dst)) {
		char c;

		if (vtb->input.drain) {
			c = vtb->input.buffer[vtb->input.index++];

			vtb->input.drain = (vtb->input.index < vtb->input.size);
		} else if (fifo_empty(src)) {
			break;
		} else {
			char u;
			const size_t r = fifo_read(src, &u, sizeof(u));

			BUG_ON(r != 1);

			c = vt_deescape_char(vtb, u, timestamp);
		}

		if (c) {
			const size_t w = fifo_write(dst, &c, sizeof(c));

			BUG_ON(w != 1);

			t++;
		}
	}

	return t;
}

void vt_clear(struct vt_buffer *vtb)
{
	const size_t area = vtb->server.size.rows * vtb->server.size.cols;
	memset(vtb->server.chars, 0, sizeof(vt_char) * area);
	memset(vtb->server.attrs, 0, sizeof(struct vt_attr) * area);

	vtb->clear = true;
}

void vt_redraw(struct vt_buffer *vtb)
{
	vtb->output.redraw = true;
}

struct vt_text vt_text_combine(const struct vt_text a, const struct vt_text b)
{
	const char *as = vt_text(a);
	const char *bs = vt_text(b);
	const size_t al = strlen(as);
	const size_t bl = strlen(bs);
	struct vt_text c;

	BUG_ON(al + bl >= sizeof(c.b));

	c.s = NULL;
	memcpy(&c.b[0], as, al);
	memcpy(&c.b[al], bs, bl);
	c.b[al + bl] = '\0';

	return c;
}

struct vt_text vt_cursor_end(struct vt_buffer *vtb)
{
	return vtb->cmd->position ?
		vtb->cmd->position(vtb->client.size.rows,
				   vtb->client.size.cols) :
		(struct vt_text) { .s = "" };
}

struct vt_text vt_reset(struct vt_buffer *vtb)
{
	vt_redraw(vtb);

	const struct vt_text reset = vtb->cmd->reset ?
		vtb->cmd->reset() : (struct vt_text) { .s = "" };
	const struct vt_text show = vtb->cmd->show ?
		vtb->cmd->show() : (struct vt_text) { .s = "" };

	return vt_text_combine(reset, show);
}

void vt_client_resize(struct vt_buffer *vtb, int rows, int cols)
{
	vtb->client.size.rows = rows;
	vtb->client.size.cols = cols;
}
