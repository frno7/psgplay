// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#ifndef PSGPLAY_OUTPUT_H
#define PSGPLAY_OUTPUT_H

#include "internal/types.h"

struct output {
	void *(*open)(const char *output, int frequency, bool nonblocking);
	bool (*sample)(s16 left, s16 right, void *arg);
	void (*drop)(void *arg);
	void (*close)(void *arg);
};

#endif /* PSGPLAY_OUTPUT_H */
