// SPDX-License-Identifier: GPL-2.0

#ifndef INTERNAL_TYPES_H
#define INTERNAL_TYPES_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>

#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#define __BITFIELD_FIELD(field, more)					\
	field;								\
	more
#elif __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define __BITFIELD_FIELD(field, more)					\
	more								\
	field;
#else
#error "bitfield neither big nor little endian?"
#endif

typedef int8_t s8;	/* FIXME: Deprecate */
typedef int16_t s16;	/* FIXME: Deprecate */
typedef int32_t s32;	/* FIXME: Deprecate */
typedef int64_t s64;	/* FIXME: Deprecate */

typedef uint8_t u8;	/* FIXME: Deprecate */
typedef uint16_t u16;	/* FIXME: Deprecate */
typedef uint32_t u32;	/* FIXME: Deprecate */
typedef uint64_t u64;	/* FIXME: Deprecate */

/* Macro definitions from the Linux kernel. */

/* Are two types/vars the same type (ignoring qualifiers)? */
#ifndef __same_type
# define __same_type(a, b) __builtin_types_compatible_p(typeof(a), typeof(b))
#endif

/*
 * min()/max()/clamp() macros must accomplish three things:
 *
 * - avoid multiple evaluations of the arguments (so side-effects like
 *   "x++" happen only once) when non-constant.
 * - perform strict type-checking (to generate warnings instead of
 *   nasty runtime surprises). See the "unnecessary" pointer comparison
 *   in __typecheck().
 * - retain result as a constant expressions when called with only
 *   constant expressions (to avoid tripping VLA warnings in stack
 *   allocation usage).
 */
#define __typecheck(x, y) (!!(sizeof((typeof(x) *)1 == (typeof(y) *)1)))

#endif /* INTERNAL_TYPES_H */
