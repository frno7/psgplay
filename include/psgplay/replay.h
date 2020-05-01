// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#ifndef PSGPLAY_REPLAY_H
#define PSGPLAY_REPLAY_H

#include "atari/machine.h"

#include "psgplay/types.h"
#include "psgplay/option.h"
#include "psgplay/output.h"

#include "system/unix/file.h"

void replay(const struct options *options, struct file file,
	const struct output *output, const struct machine *machine);

#endif /* PSGPLAY_REPLAY_H */
