// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2025 Fredrik Noring
 */

#ifndef PSGPLAY_TEST_VERIFY_H
#define PSGPLAY_TEST_VERIFY_H

#include "audio/audio.h"

#include "test/option.h"

#define verify_assert(expr) if (!(expr))

char *verify(const struct audio *audio, const struct options *options);

#endif /* PSGPLAY_TEST_VERIFY_H */
