// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#ifndef PSGPLAY_SYSTEM_UNIX_REPLAY_H
#define PSGPLAY_SYSTEM_UNIX_REPLAY_H

#include "internal/types.h"

#include "atari/machine.h"

#include "psgplay/output.h"

#include "system/unix/file.h"
#include "system/unix/option.h"

void replay(const struct options *options, struct file file,
	const struct output *output, const struct machine *machine);

struct file sndh_read_file(const char *path);

#endif /* PSGPLAY_SYSTEM_UNIX_REPLAY_H */
