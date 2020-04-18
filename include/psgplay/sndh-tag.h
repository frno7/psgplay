// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#ifndef PSGPLAY_SNDH_TAG_H
#define PSGPLAY_SNDH_TAG_H

#include "psgplay/file.h"
#include "psgplay/types.h"

bool sndh_tag_subtune_count(int *track_count, struct file file);

bool sndh_tag_default_subtune(int *track, struct file file);

bool sndh_tag_time(float *duration, struct file file);

#endif /* PSGPLAY_SNDH_TAG_H */
