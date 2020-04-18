// SPDX-License-Identifier: GPL-2.0

#ifndef PSGPLAY_ASSERT_H
#define PSGPLAY_ASSERT_H

#include "psgplay/print.h"
#include "psgplay/types.h"

/* Macro definitions from the Linux kernel. */

#define __compiletime_error(message) __attribute__((__error__(message)))

#ifdef __OPTIMIZE__
# define __compiletime_assert(condition, msg, prefix, suffix)		\
	do {								\
		extern void prefix ## suffix(void) __compiletime_error(msg); \
		if (!(condition))					\
			prefix ## suffix();				\
	} while (0)
#else
# define __compiletime_assert(condition, msg, prefix, suffix) do { } while (0)
#endif

#define _compiletime_assert(condition, msg, prefix, suffix)		\
	__compiletime_assert(condition, msg, prefix, suffix)

/**
 * compiletime_assert - break build and emit msg if condition is false
 * @condition: a compile-time constant condition to check
 * @msg:       a message to emit if condition is false
 *
 * In tradition of POSIX assert, this macro will break the build if the
 * supplied condition is *false*, emitting the supplied error message if the
 * compiler has support to do so.
 */
#define compiletime_assert(condition, msg)				\
	_compiletime_assert(condition, msg, __compiletime_assert_, __LINE__)

#define BUG_ON(expr)							\
	do {								\
		if (expr)						\
			pr_bug(__FILE__, __LINE__, __func__, #expr);	\
	} while (0)

#define BUG()								\
	do {								\
		pr_bug(__FILE__, __LINE__, __func__, "fatal error");	\
	} while (0)

#define WARN(format...)							\
	do {								\
		pr_bug_warn(__FILE__, __LINE__, __func__, format);	\
	} while (0)

#define WARN_ONCE(format...)						\
	do {								\
		static bool warned__;					\
		if (!warned__)						\
			WARN(format);					\
		warned__ = true;					\
	} while (0)

#endif /* PSGPLAY_ASSERT_H */
