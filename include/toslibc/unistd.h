// SPDX-License-Identifier: LGPL-2.1

#ifndef _TOSLIBC_UNISTD_H
#define _TOSLIBC_UNISTD_H

#include <stddef.h>

#include "toslibc/tos/gemdos.h"

#define STDIN_FILENO	0
#define STDOUT_FILENO	1
#define STDERR_FILENO	2

off_t lseek(int fd, off_t offset, int whence);

ssize_t read(int fd, void *buf, size_t count);

ssize_t write(int fd, const void *buf, size_t count);

int close(int fd);

#endif /* _TOSLIBC_UNISTD_H */
