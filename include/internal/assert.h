// SPDX-License-Identifier: GPL-2.0

#ifndef INTERNAL_ASSERT_H
#define INTERNAL_ASSERT_H

#include <stdbool.h>

#include "internal/build-assert.h"
#include "internal/print.h"

/* Macro definitions from the Linux kernel. */

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

#endif /* INTERNAL_ASSERT_H */
