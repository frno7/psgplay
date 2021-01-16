// SPDX-License-Identifier: LGPL-2.1
/*
 * Copyright (C) 2020 Fredrik Noring
 */

#include <errno.h>
#include <stdlib.h>

#include <tos/gemdos.h>

#include "internal/malloc.h"

void free(void *ptr)
{
	if (!ptr)
		return;

	struct __malloc *m = ptr;

	gemdos_mfree(&m[-1]);
}
