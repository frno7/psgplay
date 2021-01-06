// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#ifndef PSGPLAY_SYSTEM_UNIX_DISASSEMBLE_H
#define PSGPLAY_SYSTEM_UNIX_DISASSEMBLE_H

#include "system/unix/file.h"
#include "system/unix/option.h"

void sndh_disassemble(struct options *options, struct file file);

void sndh_trace(struct options *options, struct file file);

#endif /* PSGPLAY_SYSTEM_UNIX_DISASSEMBLE_H */
