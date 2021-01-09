// SPDX-License-Identifier: LGPL-2.1
/*
 * Copyright (C) 2021 Fredrik Noring
 */

#include <tos/basepage.h>
#include <tos/gemdos.h>

#include "internal/build-assert.h"

void __NORETURN gemdos_ptermres(int16_t code)
{
	const size_t size =
		sizeof(*_basepage) +
		_basepage->text.size +
		_basepage->data.size +
		_basepage->bss.size;

	BUILD_BUG_ON(sizeof(*_basepage) != 256);

	gemdos_ptermres__(size, code);
}
