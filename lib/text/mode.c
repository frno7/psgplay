// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#include <stdbool.h>
#include <string.h>

#include "psgplay/sndh.h"

#include "unicode/utf8.h"

#include "text/mode.h"

unicode_t fifo_utf32(struct fifo_utf32 *ffu)
{
	const unicode_t t = utf8_to_utf32_next(&ffu->uua);

	if (t)
		return t;

	char c;

	if (!fifo_read(&ffu->fifo, &c, sizeof(c)))
		return 0;

	return utf8_to_utf32_first(&ffu->uua, c);
}

struct text_sndh text_sndh_init(const char *title,
	const char *path, const void *data, size_t size)
{
	struct text_sndh sndh = {
		.path = path,
		.size = size,
		.data = data,
	};

	if (!sndh_tag_title(sndh.title, sizeof(sndh.title), data, size)) {
		strncpy(sndh.title, title, sizeof(sndh.title) - 1);
		sndh.title[sizeof(sndh.title) - 1] = '\0';
	}

	if (!sndh_tag_subtune_count(&sndh.subtune_count, data, size))
		sndh.subtune_count = 1;

	return sndh;
}
