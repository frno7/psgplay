// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#include <string.h>

#include "internal/compare.h"
#include "internal/fifo.h"

size_t fifo_write(struct fifo *f, const void *buf, size_t size)
{
	const size_t s = min(f->capacity - f->size, size);
	const size_t i = f->index + f->size < f->capacity ?
		f->index + f->size : f->index + f->size - f->capacity;
	const u8 *src = buf;
	u8 *dst = f->buffer;

	if (!s)
		return 0;

	if (i + s > f->capacity) {
		const size_t p = f->capacity - i;

		memcpy(&dst[i], &src[0], p);
		memcpy(&dst[0], &src[p], s - p);
	} else
		memcpy(&dst[i], &src[0], s);

	f->size += s;

	return s;
}

size_t fifo_read(struct fifo *f, void *buf, size_t size)
{
	const size_t s = min(f->size, size);
	const u8 *src = f->buffer;
	u8 *dst = buf;

	if (!s)
		return 0;

	if (f->index + s > f->capacity) {
		const size_t p = f->capacity - f->index;

		memcpy(&dst[0], &src[f->index], p);
		memcpy(&dst[p], &src[0], s - p);
	} else
		memcpy(&dst[0], &src[f->index], s);

	f->index += s;
	if (f->index >= f->capacity)
		f->index -= f->capacity;

	f->size -= s;

	return s;
}

size_t fifo_peek(struct fifo *f, const void **buf)
{
	const u8 *src = f->buffer;

	if (buf != NULL)
		*buf = &src[f->index];

	return f->index + f->size > f->capacity ?
		f->capacity - f->index : f->size;
}

size_t fifo_skip(struct fifo *f, size_t size)
{
	const size_t s = min(f->size, size);

	f->index += s;
	if (f->index >= f->capacity)
		f->index -= f->capacity;

	f->size -= s;

	return s;
}
