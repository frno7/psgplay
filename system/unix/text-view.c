// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#include <stdbool.h>

#include "unicode/utf8.h"

#include "system/unix/text-view.h"

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
