// SPDX-License-Identifier: GPL-2.0

#ifndef PSGPLAY_PRINT_H
#define PSGPLAY_PRINT_H

#include <stdarg.h>
#include <stdio.h>

#include "psgplay/macro.h"
#include "psgplay/sha256.h"

void pr_info(const char *fmt, ...)
	__attribute__((format(printf, 1, 2)));

void pr_warn(const char *msg, ...)
	__attribute__((format(printf, 1, 2)));

void pr_warn_errno(const char *s);

void pr_errno(const char *s);

void pr_error(const char *fmt, ...);

void NORETURN pr_fatal_error(const char *fmt, ...)
	__attribute__((format(printf, 1, 2)));

void NORETURN pr_fatal_errno(const char *s);

void pr_bug_warn(const char *file, int line,
	const char *func, const char *fmt, ...);

void NORETURN pr_bug(const char *file, int line,
	const char *func, const char *expr);

void pr_sha256(struct sha256 sha256);

void pr_mem(FILE *f, const void *data, size_t size, size_t offset);

#endif /* PSGPLAY_PRINT_H */
