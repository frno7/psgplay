// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#include <tos/system-variable.h>

struct system_variables *__system_variables =
	(struct system_variables *)0x400;
