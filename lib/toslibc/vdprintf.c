// SPDX-License-Identifier: GPL-2.0

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int vdprintf(int fd, const char *format, va_list ap)
{
	char buf[256];
	va_list aq;

	va_copy(aq, ap);

	int r = vsnprintf(buf, sizeof(buf), format, ap);

	if (r <= 0) {
		return r;
	} else if (r + 1 <= sizeof(buf)) {
		r = write(fd, buf, r);
	} else {
		char *b = malloc(r + 1);

		if (!b) {
			r = -1;
			goto out;
		}

		r = vsnprintf(b, r + 1, format, aq);

		if (r > 0)
			r = write(fd, b, r);

		free(b);
	}

out:
	va_end(aq);

	return r;
}
