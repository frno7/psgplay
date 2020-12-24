// SPDX-License-Identifier: LGPL-2.1

#ifndef _TOSLIBC_FCNTL_H
#define _TOSLIBC_FCNTL_H

#include "toslibc/tos/gemdos.h"

#define O_RDONLY	S_READ
#define O_WRONLY	S_WRITE
#define O_RDWR		S_READWRITE

int open(const char *pathname, int flags);

#endif /* _TOSLIBC_FCNTL_H */
