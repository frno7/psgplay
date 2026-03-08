// SPDX-License-Identifier: GPL-2.0

#ifndef PSGPLAY_UNICODE_H
#define PSGPLAY_UNICODE_H

#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>

typedef uint32_t unicode_t;

int utf8_to_utf32(unicode_t *u, const uint8_t *s, size_t insize);

int utf32_to_utf8(unicode_t u, uint8_t *s, size_t maxout);

int utf32_to_utf8_length(unicode_t u);

int utf32_to_utf8_with_replacement(unicode_t u, uint8_t *s, size_t outmax);

ssize_t charset_to_utf8_string_length(const uint8_t *s, size_t length,
	 unicode_t (*charset_to_utf32)(uint8_t c, void *arg), void *arg);

ssize_t utf8_to_charset_string_length(const uint8_t *u, size_t length);

bool utf8_valid_in_charset_string(const uint8_t *u, size_t length,
	 unicode_t (*charset_to_utf32)(uint8_t c, void *arg),
	 uint8_t (*utf32_to_charset)(unicode_t u, void *arg), void *arg);

struct utf8_to_utf32_adapter {
	size_t length;
	uint8_t s[6];		/* Longest UTF-8 sequence */
};

unicode_t utf8_to_utf32_first(struct utf8_to_utf32_adapter *uua, char c);

unicode_t utf8_to_utf32_next(struct utf8_to_utf32_adapter *uua);

#endif /* PSGPLAY_UNICODE_H */
