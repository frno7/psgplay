// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#include "internal/macro.h"

#include "toslibc/tos/system-variable.h"

struct system_variable {
	u32 address;
	u32 count;
	u32 size;
	const char *label;
	const char *description;
};

static bool valid_system_variable(u32 address,
	const struct system_variable *sv)
{
	return sv->address <= address &&
			      address < sv->address + sv->count * sv->size;
}

const char *system_variable_label(u32 address)
{
	static const struct system_variable list[] = {
#define SYSTEM_VARIABLE_DESCRIPTION(					\
		address_, count_, size_, label_, type_, description_)	\
		{ address_, count_, size_, #label_, description_ },
SYSTEM_VARIABLE(SYSTEM_VARIABLE_DESCRIPTION)
	};
	const size_t n = ARRAY_SIZE(list);
	const size_t s = list[0].address;
	const size_t e = list[n - 1].address +
		list[n - 1].count * list[n - 1].size;

	if (s <= address && address < e)
		for (size_t i = 0; i < n; i++)
			if (valid_system_variable(address, &list[i]))
				return list[i].label;

	return "";
}
