// SPDX-License-Identifier: LGPL-2.1
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
