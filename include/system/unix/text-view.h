// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#ifndef PSGPLAY_SYSTEM_UNIX_TEXT_VIEW_H
#define PSGPLAY_SYSTEM_UNIX_TEXT_VIEW_H

#include "internal/types.h"

#include "unicode/utf8.h"

#include "vt/vt.h"

struct fifo_utf32 {
	u8 buffer[64];
	struct fifo fifo;
	struct utf8_to_utf32_adapter uua;
};

#define DEFINE_FIFO_UTF32(id_)						\
	struct fifo_utf32 id_ = INIT_FIFO(id_)

struct text_view {
	void (*init)(struct vt_buffer *vtb);
	void (*event)(struct vt_buffer *vtb, struct fifo_utf32 *in_utf32);
};

unicode_t fifo_utf32(struct fifo_utf32 *ffu);

#define for_each_fifo_utf32(symbol, ffu)				\
	for (symbol = fifo_utf32(ffu);					\
	     symbol;							\
	     symbol = fifo_utf32(ffu))

#define UNICODE_KEYS(k)							\
	k(0x2190, ARROW_LEFT)						\
	k(0x2191, ARROW_UP)						\
	k(0x2192, ARROW_RIGHT)						\
	k(0x2193, ARROW_DOWN)

enum unicode_code {
#define DEFINE_UNICODE_ENUM(code, label)				\
	U_##label = code,
UNICODE_KEYS(DEFINE_UNICODE_ENUM)
};

#endif /* PSGPLAY_SYSTEM_UNIX_TEXT_VIEW_H */
