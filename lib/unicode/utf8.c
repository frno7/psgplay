// SPDX-License-Identifier: GPL-2.0

#define _POSIX_C_SOURCE 200809L

#include <limits.h>
#include <stdlib.h>

#include "internal/types.h"

#include "psgplay/assert.h"
#include "psgplay/print.h"

#include "unicode/utf8.h"

#include "system/unix/memory.h"
#include "system/unix/string.h"

struct utf8_table {
	u32 cmask;
	u32 cval;
	u32 shift;
	u32 lmask;
	u32 lval;
};

static const struct utf8_table utf8_table[] =
{
	{ 0x80, 0x00, 0*6, 0x7f,       0         },	/* 1 byte sequence */
	{ 0xe0, 0xc0, 1*6, 0x7ff,      0x80      },	/* 2 byte sequence */
	{ 0xf0, 0xe0, 2*6, 0xffff,     0x800     },	/* 3 byte sequence */
	{ 0xf8, 0xf0, 3*6, 0x1fffff,   0x10000   },	/* 4 byte sequence */
	{ 0xfc, 0xf8, 4*6, 0x3ffffff,  0x200000  },	/* 5 byte sequence */
	{ 0xfe, 0xfc, 5*6, 0x7fffffff, 0x4000000 },	/* 6 byte sequence */
	{ 0 }						/* End of table */
};

#define UNICODE_MAX	0x0010ffff
#define PLANE_SIZE	0x00010000

#define SURROGATE_MASK	0xfffff800
#define SURROGATE_PAIR	0x0000d800
#define SURROGATE_LOW	0x00000400
#define SURROGATE_BITS	0x000003ff

#define UNICODE_REPLACEMENT_CHARACTER 0xfffd

int utf8_to_utf32(unicode_t *u, const u8 *s, size_t insize)
{
	if (!insize)
		return 0;

	const u32 c0 = *s;
	u32 code = c0;
	int length = 0;
	for (const struct utf8_table *t = utf8_table; t->cmask; t++) {
		length++;

		if ((c0 & t->cmask) == t->cval) {
			code &= t->lmask;
			if (code < t->lval || UNICODE_MAX < code ||
					(code & SURROGATE_MASK) == SURROGATE_PAIR)
				return -1;
			if (u)
				*u = (unicode_t)code;
			return length;
		}

		if (insize <= length)
			return -1;

		s++;

		const u32 c = (*s ^ 0x80) & 0xff;
		if (c & 0xc0)
			return -1;
		code = (code << 6) | c;
	}

	return -1;
}

int utf32_to_utf8(unicode_t u, u8 *s, size_t outmax)
{
	const struct utf8_table *t;
	int length = 0;

	if (UNICODE_MAX < u || (u & SURROGATE_MASK) == SURROGATE_PAIR)
		return -1;

	for (t = utf8_table; t->cmask && outmax; t++, outmax--) {
		length++;

		if (u <= t->lmask) {
			int c = t->shift;

			*s = (u8)(t->cval | (u >> c));

			while (c > 0) {
				c -= 6;
				*++s = (u8)(0x80 | ((u >> c) & 0x3f));
			}

			return length;
		}
	}

	return -1;
}

static int utf32_to_utf8_with_replacement(unicode_t u, u8 *s, size_t outmax)
{
	const int r = utf32_to_utf8(u, s, outmax);

	return r != -1 ? r :
		utf32_to_utf8(UNICODE_REPLACEMENT_CHARACTER, s, outmax);

}

int utf32_to_utf8_length(unicode_t u)
{
	u8 s[8];

	return utf32_to_utf8(u, s, sizeof(s));
}

static ssize_t charset_to_utf8_string_length(const u8 *s, size_t length,
	 unicode_t (*charset_to_utf32)(u8 c, void *arg), void *arg)
{
	ssize_t size = 0;

	for (size_t i = 0; s[i] && i < length; i++) {
		int r = utf32_to_utf8_length(charset_to_utf32(s[i], arg));

		if (r == -1)
			r = utf32_to_utf8_length(UNICODE_REPLACEMENT_CHARACTER);

		if (r == -1)
			return -1;

		size += r;
	}

	return size;
}

u8 *charset_to_utf8_string(const u8 *s, size_t length,
	 unicode_t (*charset_to_utf32)(u8 c, void *arg), void *arg)
{
	const ssize_t size = charset_to_utf8_string_length(s, length,
			charset_to_utf32, arg);

	if (size == -1)
		return NULL;

	u8 *u = xmalloc(size + 1);

	size_t n = 0;

	for (size_t i = 0; s[i] && i < length; i++) {
		const int r = utf32_to_utf8_with_replacement(
				charset_to_utf32(s[i], arg), &u[n], size - n);

		if (r == -1) {
			free(u);
			return NULL;
		}

		n += r;
	}

	BUG_ON(n != size);

	u[n] = '\0';

	return u;
}

static ssize_t utf8_to_charset_string_length(const u8 *u, size_t length)
{
	ssize_t size = 0;
	size_t i = 0;

	while (u[i] && i < length) {
		int r = utf8_to_utf32(NULL, &u[i], length - i);

		if (r != -1) {
			i += r;
			size++;
		} else
			i++;
	}

	return size;
}

u8 *utf8_to_charset_string(const u8 *u, size_t length,
	 u8 (*utf32_to_charset)(unicode_t u, void *arg), void *arg)
{
	const ssize_t size = utf8_to_charset_string_length(u, length);

	if (size == -1)
		return NULL;

	u8 *s = xmalloc(size + 1);

	size_t n = 0;
	size_t i = 0;

	while (u[i] && i < length) {
		unicode_t c = 0;

		int r = utf8_to_utf32(&c, &u[i], length - i);

		if (r == -1) {
			i++;
			continue;
		}

		s[n++] = utf32_to_charset(c, arg);
		i += r;
	}

	BUG_ON(n != size);

	s[n] = '\0';

	return s;
}

bool utf8_valid_in_charset_string(const u8 *u, size_t length,
	 unicode_t (*charset_to_utf32)(u8 c, void *arg),
	 u8 (*utf32_to_charset)(unicode_t u, void *arg), void *arg)
{
	size_t i = 0;

	while (u[i] && i < length) {
		unicode_t c = 0;

		int r = utf8_to_utf32(&c, &u[i], length - i);

		i += r != -1 ? r : 1;

		if (charset_to_utf32(utf32_to_charset(c, arg), arg) != c)
			return false;
	}

	return true;
}
