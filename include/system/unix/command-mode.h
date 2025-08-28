// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#ifndef PSGPLAY_SYSTEM_UNIX_COMMAND_MODE_H
#define PSGPLAY_SYSTEM_UNIX_COMMAND_MODE_H

#include "internal/types.h"

#include "audio/writer.h"

#include "system/unix/file.h"
#include "system/unix/option.h"

void command_replay(const struct options *options, struct file file,
	const struct audio_writer *output);

#endif /* PSGPLAY_SYSTEM_UNIX_COMMAND_MODE_H */
