// SPDX-License-Identifier: LGPL-2.1

#ifndef _TOSLIBC_FCNTL_H
#define _TOSLIBC_FCNTL_H

#include <tos/gemdos.h>

#define O_RDONLY	S_READ
#define O_WRONLY	S_WRITE
#define O_RDWR		S_READWRITE

#define O_CREAT		0	/* FIXME */
#define O_TRUNC		0	/* FIXME */

int open(const char *pathname, int flags, ...);

#endif /* _TOSLIBC_FCNTL_H */
