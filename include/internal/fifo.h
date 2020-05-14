// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#ifndef INTERNAL_FIFO_H
#define INTERNAL_FIFO_H

#include "internal/types.h"

struct fifo {
	size_t size;
	size_t index;
	size_t capacity;
	void *buffer;
};

#define DECLARE_FIFO(capacity_)						\
	struct {							\
		u8 buffer[capacity_];					\
		struct fifo fifo;					\
	}

#define INIT_FIFO(id_)							\
{									\
	.fifo = {							\
		.capacity = sizeof(id_.buffer),				\
		.buffer = &id_.buffer					\
	}								\
}

#define DEFINE_FIFO(id_, capacity_)					\
	DECLARE_FIFO(capacity_) id_ = INIT_FIFO(id_)

static inline size_t fifo_size(const struct fifo *f)
{
	return f->size;
}

static inline bool fifo_empty(const struct fifo *f)
{
	return fifo_size(f) == 0;
}

static inline bool fifo_full(const struct fifo *f)
{
	return fifo_size(f) == f->capacity;
}

static inline size_t fifo_remaining(const struct fifo *f)
{
	return f->capacity - fifo_size(f);
}

size_t fifo_write(struct fifo *f, const void *buf, size_t size);

size_t fifo_read(struct fifo *f, void *buf, size_t size);

size_t fifo_peek(struct fifo *f, const void **buf);

size_t fifo_skip(struct fifo *f, size_t size);

#endif /* INTERNAL_FIFO_H */
