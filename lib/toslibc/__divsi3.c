// SPDX-License-Identifier: GCC-exception-2.0

#include <stdint.h>
#include <stddef.h>

extern uint32_t __udivmodsi4(uint32_t num, uint32_t den, uint32_t * rem);

int32_t __divsi3(int32_t num, int32_t den)
{
	int minus = 0;
	int32_t v;

	if (num < 0) {
		num = -num;
		minus = 1;
	}
	if (den < 0) {
		den = -den;
		minus ^= 1;
	}

	v = __udivmodsi4(num, den, NULL);
	if (minus)
		v = -v;

	return v;
}
