// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#ifndef PSGPLAY_SYSTEM_ATARI_MODEL_H
#define PSGPLAY_SYSTEM_ATARI_MODEL_H

#include "psgplay/sndh.h"

#include "text/mvc.h"

void model_timer(struct text_sndh *sndh);

void model_update(struct text_state *model, const struct text_state *ctrl,
	struct text_sndh *sndh, u64 timestamp);

#endif /* PSGPLAY_SYSTEM_ATARI_MODEL_H */
