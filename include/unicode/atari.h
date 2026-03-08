// SPDX-License-Identifier: GPL-2.0

#ifndef PSGPLAY_UNICODE_ATARI_ST_H
#define PSGPLAY_UNICODE_ATARI_ST_H

#include <stdbool.h>

#include "unicode/utf8.h"

unicode_t charset_atari_st_to_utf32(uint8_t c, void *arg);

uint8_t utf32_to_charset_atari_st(unicode_t u, void *arg);

bool utf8_valid_in_atari_st(const uint8_t *u, size_t length);

#endif /* PSGPLAY_UNICODE_ATARI_ST_H */
