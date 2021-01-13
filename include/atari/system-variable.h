// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#ifndef ATARI_SYSTEM_VARIABLE_H
#define ATARI_SYSTEM_VARIABLE_H

#include "internal/types.h"

#include "fujilibc/tos/system-variable.h"

const char *system_variable_label(u32 address);

#endif /* ATARI_SYSTEM_VARIABLE_H */
