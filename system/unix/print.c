// SPDX-License-Identifier: GPL-2.0

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "internal/compare.h"
#include "internal/macro.h"
#include "internal/print.h"
#include "internal/types.h"

#include "system/unix/print.h"

static void pr_printables(FILE *f,
	size_t offset, size_t columns, size_t size, const u8 *b)
{
	const size_t d = size - offset;
	for (size_t i = 0; i < (d < columns ? columns - d : 0); i++)
		fprintf(f, "   ");
	fprintf(f, " ");

	for (size_t i = 0; i < min(columns, size - offset); i++)
		fprintf(f, "%c", isprint(b[offset + i]) ? b[offset + i] : '.');
}

void pr_mem(FILE *f, const void *data, size_t size, size_t offset)
{
	const int columns = 16;
	const u8 *b = data;

	for (size_t i = 0; i < size; i++) {
		char address[32];

		sprintf(address, "\n\t%06zx ", offset + i);

		fprintf(f, "%s%02x",
			!i ? &address[1] : i % columns == 0 ?  &address[0] : " ",
			b[i]);

		if ((i + 1) % columns == 0 || i + 1 == size)
			pr_printables(f, i - (i % columns), columns, size, b);
	}
}
