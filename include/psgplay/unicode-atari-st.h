// SPDX-License-Identifier: GPL-2.0

#ifndef PSGPLAY_UNICODE_ATARI_ST_H
#define PSGPLAY_UNICODE_ATARI_ST_H

#include "psgplay/types.h"
#include "psgplay/unicode.h"

unicode_t charset_atari_st_to_utf32(u8 c, void *arg);

u8 utf32_to_charset_atari_st(unicode_t u, void *arg);

bool utf8_valid_in_atari_st(const u8 *u, size_t length);

#endif /* PSGPLAY_UNICODE_ATARI_ST_H */
