// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2020 Fredrik Noring
 */

#include <errno.h>
#include <stdlib.h>

#include "toslibc/tos/gemdos.h"

void free(void *ptr)
{
	if (ptr)
		gemdos_mfree(ptr);
}
