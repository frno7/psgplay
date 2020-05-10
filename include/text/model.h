// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#ifndef PSGPLAY_TEXT_MODEL_H
#define PSGPLAY_TEXT_MODEL_H

struct text_model {
	char title[40];
	int subtune_count;

	struct {
		int index;
	} cursor;

	struct {
		int track;
		enum {
			TRACK_STOP,
			TRACK_PLAY,
			TRACK_PAUSE,
			TRACK_RESTART,
		} state;
	} ctrl, view;

	struct {
		const char *path;
		size_t size;
		void *data;
	} sndh;
};

#endif /* PSGPLAY_TEXT_MODEL_H */
