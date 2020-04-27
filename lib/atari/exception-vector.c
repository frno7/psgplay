// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#include "atari/exception-vector.h"

const char *exception_vector_description(u32 address)
{
	switch (address >> 2) {
#define EXCEPTION_VECTOR_DESCRIPTION(vector_, description_)		\
	case vector_: return description_;
EXCEPTION_VECTOR(EXCEPTION_VECTOR_DESCRIPTION)
	default: return "";
	}
}
