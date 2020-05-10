// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#ifndef PSGPLAY_SYSTEM_UNIX_TTY_H
#define PSGPLAY_SYSTEM_UNIX_TTY_H

#include <stdbool.h>
#include <termios.h>
#include <unistd.h>

#include "system/unix/file.h"

struct tty_size {
	int rows;
	int cols;
};

struct tty_events {
	void (*resize)(struct tty_size size, void *arg);
	void (*suspend)(void *arg);
	void (*resume)(void *arg);
	void *arg;
};

struct tty_size tty_size(void);

bool tty_present(void);

bool tty_init(const struct tty_events *events);

void tty_exit(void);

#endif /* PSGPLAY_SYSTEM_UNIX_TTY_H */
