// SPDX-License-Identifier: GPL-2.0

#include <stdio.h>
#include <stdlib.h>

#include <tos/gemdos.h>

#include "internal/macro.h"

#include "psgplay/print.h"

#include "system/atari/option.h"

void NORETURN pr_bug(const char *file, int line,
	const char *func, const char *expr)
{
	printf("%s: BUG: %s:%d: %s: %s\n",
		progname, file, line, func, expr);

	gemdos_cconin();

	exit(EXIT_FAILURE);
}
