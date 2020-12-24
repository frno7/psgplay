// SPDX-License-Identifier: LGPL-2.1

#ifndef INTERNAL_LIMITS_H
#define INTERNAL_LIMITS_H

#include "internal/types.h"

#define USHRT_MAX	((unsigned short)~0U)
#define SHRT_MAX	((short)(USHRT_MAX >> 1))
#define SHRT_MIN	((short)(-SHRT_MAX - 1))
#define INT_MAX		((int)(~0U >> 1))
#define INT_MIN		(-INT_MAX - 1)
#define UINT_MAX	(~0U)
#define LONG_MAX	((long)(~0UL >> 1))
#define LONG_MIN	(-LONG_MAX - 1)
#define ULONG_MAX	(~0UL)
#define LLONG_MAX	((long long)(~0ULL >> 1))
#define LLONG_MIN	(-LLONG_MAX - 1)
#define ULLONG_MAX	(~0ULL)
#define SIZE_MAX	(~(size_t)0)
#define PHYS_ADDR_MAX	(~(phys_addr_t)0)

#define U8_MAX		((uint8_t)~0U)
#define S8_MAX		((int8_t)(U8_MAX >> 1))
#define S8_MIN		((int8_t)(-S8_MAX - 1))
#define U16_MAX		((uint16_t)~0U)
#define S16_MAX		((int16_t)(U16_MAX >> 1))
#define S16_MIN		((int16_t)(-S16_MAX - 1))
#define U32_MAX		((uint32_t)~0U)
#define S32_MAX		((int32_t)(U32_MAX >> 1))
#define S32_MIN		((int32_t)(-S32_MAX - 1))
#define U64_MAX		((uint64_t)~0ULL)
#define S64_MAX		((int64_t)(U64_MAX >> 1))
#define S64_MIN		((int64_t)(-S64_MAX - 1))

#endif /* INTERNAL_LIMITS_H */
