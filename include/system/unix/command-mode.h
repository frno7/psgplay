// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#ifndef PSGPLAY_SYSTEM_UNIX_COMMAND_MODE_H
#define PSGPLAY_SYSTEM_UNIX_COMMAND_MODE_H

#include "internal/types.h"

#include "atari/machine.h"

#include "psgplay/output.h"

#include "system/unix/file.h"
#include "system/unix/option.h"

void command_replay(const struct options *options, struct file file,
	const struct output *output, const struct machine *machine);

#endif /* PSGPLAY_SYSTEM_UNIX_COMMAND_MODE_H */
