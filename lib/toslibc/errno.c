// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2020 Fredrik Noring
 */

#include <errno.h>
#include <tos/error.h>

int errno;

int errno_for_tos_error(int toserr)
{
	switch (toserr) {
#define _TOS_ERROR_ERRNO(identifier, number, errno, description)	\
	case -TOS_E##identifier: return -errno;
	_TOS_ERRORS(_TOS_ERROR_ERRNO)
	}

	return -1000 < toserr && toserr < 0 ? -EINVAL : toserr;
}

const char *tos_error_message(int toserr)
{
	switch (toserr) {
#define _TOS_ERROR_MSG(identifier, number, errno, description)		\
	case TOS_E##identifier: return description;
	_TOS_ERRORS(_TOS_ERROR_MSG)
	}

	return "Unknown error";
}
