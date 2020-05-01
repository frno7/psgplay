// SPDX-License-Identifier: GPL-2.0

#include <ctype.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "internal/macro.h"
#include "internal/types.h"

#include "psgplay/compare.h"
#include "psgplay/print.h"

#include "system/unix/option.h"
#include "system/unix/string.h"

static void report(const char *prefix, const char *suffix,
	const char *fmt, va_list ap)
{
	char msg[4096];

	vsnprintf(msg, sizeof(msg), fmt, ap);

	fprintf(stderr, "%s: %s%s%s%s%s", progname,
		prefix, prefix[0] ? ": " : "",
		suffix, suffix[0] ? ": " : "",
		msg);
}

void pr_info(const char *fmt, ...)
{
	if (option_verbosity()) {
		char msg[4096];
		va_list ap;

		va_start(ap, fmt);
		vsnprintf(msg, sizeof(msg), fmt, ap);
		printf("%s", msg);
		va_end(ap);
	}
}

void pr_warn(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	report("warning", "", fmt, ap);
	va_end(ap);
}

void pr_warn_errno(const char *s)
{
	pr_warn("%s: %s\n", s, strerror(errno));
}

void pr_error(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	report("error", "", fmt, ap);
	va_end(ap);
}

void pr_errno(const char *s)
{
	pr_error("%s: %s\n", s, strerror(errno));
}

void NORETURN pr_fatal_error(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	report("error", "", fmt, ap);
	va_end(ap);

	exit(EXIT_FAILURE);
}

void NORETURN pr_fatal_errno(const char *s)
{
	pr_fatal_error("%s: %s\n", s, strerror(errno));
}

void pr_bug_warn(const char *file, int line,
	const char *func, const char *fmt, ...)
{
	char prefix[4096];
	va_list ap;

	snprintf(prefix, sizeof(prefix), "%s:%d: %s", file, line, func);

	va_start(ap, fmt);
	report("WARNING", prefix, fmt, ap);
	va_end(ap);
}

void NORETURN pr_bug(const char *file, int line,
	const char *func, const char *expr)
{
	fprintf(stderr, "%s: BUG: %s:%d: %s: %s\n",
		progname, file, line, func, expr);

	exit(EXIT_FAILURE);
}

static void pr_printables(FILE *f,
	size_t offset, size_t columns, size_t size, const u8 *b)
{
	const size_t d = size - offset;
	for (size_t i = 0; i < (d < columns ? columns - d : 0); i++)
		fprintf(f, "   ");
	fprintf(f, " ");

	for (size_t i = 0; i < min(columns, size - offset); i++)
		fprintf(f, "%c", isprint(b[offset + i]) ? b[offset + i] : '.');
}

void pr_mem(FILE *f, const void *data, size_t size, size_t offset)
{
	const int columns = 16;
	const u8 *b = data;

	for (size_t i = 0; i < size; i++) {
		char address[32];

		sprintf(address, "\n\t%06zx ", offset + i);

		fprintf(f, "%s%02x",
			!i ? &address[1] : i % columns == 0 ?  &address[0] : " ",
			b[i]);

		if ((i + 1) % columns == 0 || i + 1 == size)
			pr_printables(f, i - (i % columns), columns, size, b);
	}
}
