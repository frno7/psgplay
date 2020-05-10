// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#include <errno.h>
#include <poll.h>
#include <unistd.h>

#include "internal/compare.h"

#include "psgplay/assert.h"
#include "psgplay/print.h"

#include "system/unix/poll-fifo.h"

static bool fifo_ready(struct fifo *f)
{
	return f && !fifo_full(f);
}

bool poll_fifo(const struct poll_fifo *pfs, size_t n, int timeout)
{
	struct pollfd pfds[n];

	for (size_t i = 0; i < n; i++)
		pfds[i] = (struct pollfd) {
			.fd = pfs[i].fd,
			.events = (fifo_ready(pfs[i].in)  ? POLLIN  : 0) |
				  (fifo_ready(pfs[i].out) ? POLLOUT : 0)
		};

	const int p = poll(pfds, n, timeout);

	if (p == -1) {
		if (errno != EINTR)
			pr_fatal_errno("poll_fifo");

		return false;
	}

	for (size_t i = 0; i < n; i++)
		if (pfds[i].revents & (POLLERR | POLLHUP | POLLNVAL))
			pr_fatal_error("poll_fifo\n");

	for (size_t i = 0; i < n; i++) {
		if (pfs[i].in && (pfds[i].revents & POLLIN)) {
			u8 buffer[1024];

			const size_t s = min(sizeof(buffer),
				fifo_remaining(pfs[i].in));
			const size_t r = read(pfs[i].fd, buffer, s);

			if (r == -1) {
				if (errno != EAGAIN &&
				    errno != EWOULDBLOCK &&
				    errno != EINTR)
					pr_fatal_errno("poll_fifo:read");
			}

			const size_t w = fifo_write(pfs[i].in, buffer, r);

			BUG_ON(w != r);
		}

		if (pfs[i].out && (pfds[i].revents & POLLOUT)) {
			const void *buffer;
			const size_t r = fifo_peek(pfs[i].out, &buffer);
			const size_t w = write(pfs[i].fd, buffer, r);

			if (w == -1) {
				if (errno != EAGAIN &&
				    errno != EWOULDBLOCK &&
				    errno != EINTR)
					pr_fatal_errno("poll_fifo:write");
			} else
				fifo_skip(pfs[i].out, w);
		}
	}

	return true;
}
