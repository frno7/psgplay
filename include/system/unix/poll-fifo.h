// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#ifndef SYSTEM_UNIX_POLL_INFO_H
#define SYSTEM_UNIX_POLL_INFO_H

#include "internal/fifo.h"

struct poll_fifo {
	int fd;
	struct fifo *in;
	struct fifo *out;
};

bool poll_fifo(const struct poll_fifo *pfs, size_t n, int timeout);

#endif /* SYSTEM_UNIX_POLL_INFO_H */
