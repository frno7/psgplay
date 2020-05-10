// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#ifndef VT_H
#define VT_H

#include "internal/compare.h"
#include "internal/fifo.h"

#include "vt-cmd.h"

typedef char vt_char;

struct vt_attr {
	union {
		struct {
			u8 reverse : 1;
		};
		u8 state;
	};
};

struct vt_char_attr {
	vt_char code;
	struct vt_attr attr;
};

struct vt_row {		/* FIXME */
	int begin;
	int end;
};

struct vt_col {		/* FIXME */
	int begin;
	int end;
};

struct vt_buffer {
	bool clear;
	struct {
		struct {
			int rows;
			int cols;
		} size;
		vt_char *chars;
		struct vt_attr *attrs;
	} client, server;
	struct {
		int row;
		int col;
		struct vt_attr attr;
	} cursor;
	struct {
		int row;
		int col;
		size_t offset;
		size_t index;
		size_t size;
		u8 chars[32];
		bool redraw;
	} output;
	struct {
		bool escape;
		bool drain;
		u64 timestamp;
		size_t size;
		size_t index;
		char buffer[16];
	} input;
	const struct vt_cmd *cmd;
};

#define DECLARE_VT(rows_, cols_)					\
	struct {							\
		vt_char client_chars[rows_][cols_];			\
		vt_char server_chars[rows_][cols_];			\
		struct vt_attr client_attrs[rows_][cols_];		\
		struct vt_attr server_attrs[rows_][cols_];		\
		struct vt_buffer vtb;					\
	}

#define INIT_VT(id_, cmd_)						\
{									\
	.vtb = {							\
		.client = {						\
			.size = {					\
				.rows = ARRAY_SIZE(*id_.server_chars),	\
				.cols = ARRAY_SIZE(id_.server_chars),	\
			},						\
			.chars = &id_.client_chars[0][0],		\
			.attrs = &id_.client_attrs[0][0],		\
		},							\
		.server = {						\
			.size = {					\
				.rows = ARRAY_SIZE(*id_.client_chars),	\
				.cols = ARRAY_SIZE(id_.client_chars),	\
			},						\
			.chars = &id_.server_chars[0][0],		\
			.attrs = &id_.server_attrs[0][0],		\
		},							\
		.cmd = cmd_						\
	}								\
}

#define DEFINE_VT(id_, rows_, cols_, cmd_)				\
	DECLARE_VT(rows_, cols_) id_ = INIT_VT(id_, cmd_)

void vt_putc(struct vt_buffer *vtb, int row, int col,
	vt_char c, struct vt_attr attr);

void vt_putc_normal(struct vt_buffer *vtb, int row, int col, vt_char c);
void vt_putc_reverse(struct vt_buffer *vtb, int row, int col, vt_char c);

u8 vt_getc(struct vt_buffer *vtb);

ssize_t vt_read(struct vt_buffer *vtb, void *data, size_t count);

ssize_t vt_write_fifo(struct vt_buffer *vtb, struct fifo *f);

u64 vt_event(struct vt_buffer *vtb, u64 timestamp);

ssize_t vt_deescape_fifo(struct vt_buffer *vtb,
	struct fifo *dst, struct fifo *src, u64 timestamp);

void vt_clear(struct vt_buffer *vtb);

void vt_redraw(struct vt_buffer *vtb);

struct vt_text vt_cursor_end(struct vt_buffer *vtb);

struct vt_text vt_reset(struct vt_buffer *vtb);

void vt_client_resize(struct vt_buffer *vtb, int rows, int cols);

#endif /* VT_H */
