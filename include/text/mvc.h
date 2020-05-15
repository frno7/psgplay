// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#ifndef PSGPLAY_TEXT_MVC_H
#define PSGPLAY_TEXT_MVC_H

struct text_state {
	int cursor;
	int track;
	enum {
		TRACK_STOP,
		TRACK_PLAY,
		TRACK_PAUSE,
		TRACK_RESTART,
	} op;
	u64 timestamp;
	bool quit;
};

struct text_sndh {
	char title[40];
	int subtune_count;

	const char *path;
	size_t size;
	const void *data;
};

#endif /* PSGPLAY_TEXT_MVC_H */
