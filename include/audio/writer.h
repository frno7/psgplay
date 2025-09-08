// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#ifndef PSGPLAY_WRITER_H
#define PSGPLAY_WRITER_H

#include "internal/types.h"

struct audio_writer {
	void *(*open)(const char *output, int frequency,
		bool nonblocking, size_t sample_length);
	bool (*sample)(s16 left, s16 right, void *arg);
	bool (*pause)(void *arg);
	bool (*resume)(void *arg);
	void (*flush)(void *arg);
	void (*drop)(void *arg);
	void (*close)(void *arg);
};

#endif /* PSGPLAY_WRITER_H */
