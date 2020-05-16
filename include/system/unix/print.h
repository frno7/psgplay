// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#ifndef PSGPLAY_SYSTEM_UNIX_PRINT_H
#define PSGPLAY_SYSTEM_UNIX_PRINT_H

#include <stdio.h>

void pr_mem(FILE *f, const void *data, size_t size, size_t offset);

#endif /* PSGPLAY_SYSTEM_UNIX_PRINT_H */
