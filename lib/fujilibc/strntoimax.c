// SPDX-License-Identifier: MIT
/*
 * This file is based on klibc.
 */

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

intmax_t strntoimax(const char *nptr, char **endptr, int base, size_t n)
{
	return (intmax_t) strntoumax(nptr, endptr, base, n);
}
