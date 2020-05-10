// SPDX-License-Identifier: GPL-2.0

#ifndef PSGPLAY_UNICODE_H
#define PSGPLAY_UNICODE_H

#include "internal/types.h"

typedef u32 unicode_t;

int utf8_to_utf32(unicode_t *u, const u8 *s, size_t insize);

int utf32_to_utf8(unicode_t u, u8 *s, size_t maxout);

int utf32_to_utf8_length(unicode_t u);

ssize_t charset_to_utf8_string_length(const u8 *s, size_t length,
	 unicode_t (*charset_to_utf32)(u8 c, void *arg), void *arg);

ssize_t utf8_to_charset_string_length(const u8 *u, size_t length);

u8 *charset_to_utf8_string(const u8 *s, size_t length,
	 unicode_t (*charset_to_utf32)(u8 c, void *arg), void *arg);

u8 *utf8_to_charset_string(const u8 *u, size_t length,
	 u8 (*utf32_to_charset)(unicode_t u, void *arg), void *arg);

bool utf8_valid_in_charset_string(const u8 *u, size_t length,
	 unicode_t (*charset_to_utf32)(u8 c, void *arg),
	 u8 (*utf32_to_charset)(unicode_t u, void *arg), void *arg);

struct utf8_to_utf32_adapter {
	size_t length;
	u8 s[6];		/* Longest UTF-8 sequence */
};

unicode_t utf8_to_utf32_first(struct utf8_to_utf32_adapter *uua, char c);

unicode_t utf8_to_utf32_next(struct utf8_to_utf32_adapter *uua);

#define for_each_utf8_to_utf32(symbol, uua, c)				\
	for (symbol = utf8_to_unicode_first(uua, c);			\
	     symbol;							\
	     symbol = utf8_to_utf32_next(uua))

#endif /* PSGPLAY_UNICODE_H */
