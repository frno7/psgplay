// SPDX-License-Identifier: GPL-2.0

#ifndef INTERNAL_MACRO_H
#define INTERNAL_MACRO_H

/* Macro definitions from the Linux kernel. */

#define __ALIGN__MASK(x, mask) (((x) + (mask)) & ~(mask))
#define __ALIGN_(x, a) __ALIGN__MASK(x, (typeof(x))(a) - 1)
#define ALIGN(x, a) __ALIGN_((x), (a))

#define STR(x) #x
#define XSTR(x) STR(x)

#define CONCAT__(a, b) a ## b
#define CONCATENATE(a, b) CONCAT__(a, b)

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

/* This counts to 32. Any more, it will return the 33rd argument. */
#define __COUNT_ARGS__(							\
	 _0,  _1,  _2,  _3,  _4,  _5,  _6,  _7,  _8,  _9, _10,		\
	_11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21,		\
	_22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _n, X...) _n
#define COUNT_ARGS(X...) __COUNT_ARGS__(, ##X,				\
	32, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22,			\
	21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11,			\
	10,  9,  8,  7,  6,  5,  4,  3,  2,  1,  0)

/* Indirect macros required for expanded argument pasting, eg. __LINE__. */
#define ___PASTE(a,b) a##b
#define __PASTE(a,b) ___PASTE(a,b)

#define __UNIQUE_ID(prefix) __PASTE(__PASTE(__UNIQUE_ID_, prefix), __COUNTER__)

#define DIV_ROUND_CLOSEST_U32(x_, divisor_)(				\
{									\
	const typeof(divisor_) __d = divisor_;				\
	const uint32_t _tmp = (x_) + (__d / 2);				\
	_tmp / __d;							\
}									\
)

/*
 * This returns a constant expression while determining if an argument is
 * a constant expression, most importantly without evaluating the argument.
 * Glory to Martin Uecker <Martin.Uecker@med.uni-goettingen.de>
 */
#define __is_constexpr(x) \
	(sizeof(int) == sizeof(*(8 ? ((void *)((long)(x) * 0l)) : (int *)8)))

#define __no_side_effects(x, y) \
		(__is_constexpr(x) && __is_constexpr(y))

#define preserve(x)							\
	for (typeof(x) x__ = (x), y__ = 0; !y__; (x) = x__, y__ = !y__)

#define swap(a, b)							\
	do { typeof(a) __tmp = (a); (a) = (b); (b) = __tmp; } while (0)

#define NORETURN __attribute__((__noreturn__))

#define __mode(x) __attribute__((__mode__(x)))

#endif /* INTERNAL_MACRO_H */
