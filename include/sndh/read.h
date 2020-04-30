// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#ifndef PSGPLAY_SNDH_FILE_H
#define PSGPLAY_SNDH_FILE_H

#include "psgplay/file.h"
#include "psgplay/types.h"

struct file sndh_read_file(const char *path);

#endif /* PSGPLAY_SNDH_FILE_H */
