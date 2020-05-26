// SPDX-License-Identifier: GPL-2.0

#include <ctype.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "internal/compare.h"
#include "internal/macro.h"
#include "internal/print.h"
#include "internal/types.h"

#define PROGNAME "psgplay"

static void report(const char *prefix, const char *suffix,
	const char *fmt, va_list ap)
{
	char msg[4096];

	vsnprintf(msg, sizeof(msg), fmt, ap);

	fprintf(stderr, "%s: %s%s%s%s%s", PROGNAME,
		prefix, prefix[0] ? ": " : "",
		suffix, suffix[0] ? ": " : "",
		msg);
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
		PROGNAME, file, line, func, expr);

	exit(EXIT_FAILURE);
}
