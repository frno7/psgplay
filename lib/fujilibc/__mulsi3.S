// SPDX-License-Identifier: GCC-exception-2.0

#include <tos/stddef.S>

	.pushsection .text.__mulsi3
	.globl	__mulsi3
__mulsi3:
	move.w	4(sp),d0	/* x0 -> d0 */
	mulu.w	10(sp),d0	/* x0*y1 */
	move.w	6(sp),d1	/* x1 -> d1 */
	mulu.w	8(sp),d1	/* x1*y0 */
#if !(defined(__mcf5200__) || defined(__mcoldfire__))
	add.w	d1,d0
#else
	add.l	d1,d0
#endif
	swap	d0
	clr.w	d0
	move.w	6(sp),d1	/* x1 -> d1 */
	mulu.w	10(sp),d1	/* x1*y1 */
	add.l	d1,d0
	rts

	.popsection
