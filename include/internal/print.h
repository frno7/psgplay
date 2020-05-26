// SPDX-License-Identifier: GPL-2.0

#ifndef INTERNAL_PRINT_H
#define INTERNAL_PRINT_H

#include <stdarg.h>
#include <stdio.h>

#include "internal/macro.h"

void pr_warn(const char *fmt, ...)
	__attribute__((format(printf, 1, 2)));

void pr_warn_errno(const char *s);

void pr_errno(const char *s);

void pr_error(const char *fmt, ...)
	__attribute__((format(printf, 1, 2)));

void NORETURN pr_fatal_error(const char *fmt, ...)
	__attribute__((format(printf, 1, 2)));

void NORETURN pr_fatal_errno(const char *s);

void pr_bug_warn(const char *file, int line,
	const char *func, const char *fmt, ...);

void NORETURN pr_bug(const char *file, int line,
	const char *func, const char *expr);

#endif /* INTERNAL_PRINT_H */
