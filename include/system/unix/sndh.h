// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#ifndef PSGPLAY_SYSTEM_UNIX_SNDH_H
#define PSGPLAY_SYSTEM_UNIX_SNDH_H

#include <stdbool.h>
#include <stddef.h>

struct file sndh_read_file(const char *path);

#endif /* PSGPLAY_SYSTEM_UNIX_SNDH_H */
