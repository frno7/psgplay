// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#ifndef PSGPLAY_TEXT_MODE_H
#define PSGPLAY_TEXT_MODE_H

#include <stdbool.h>

#include "internal/types.h"

#include "text/mvc.h"

#include "unicode/utf8.h"

#include "vt/vt.h"

struct fifo_utf32 {
	u8 buffer[64];
	struct fifo fifo;
	struct utf8_to_utf32_adapter uua;
};

#define DEFINE_FIFO_UTF32(id_)						\
	struct fifo_utf32 id_ = INIT_FIFO(id_)

struct text_mode {
	u64 (*view)(struct vt_buffer *vtb, struct text_state *view,
		const struct text_state *model, const struct text_sndh *sndh,
		u64 timestamp);
	void (*ctrl)(const unicode_t key, struct text_state *ctrl,
		const struct text_state *model, const struct text_sndh *sndh);
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

struct text_sndh text_sndh_init(const char *title,
	const char *path, const void *data, size_t size);

#endif /* PSGPLAY_TEXT_MODE_H */
