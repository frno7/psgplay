// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#ifndef PSGPLAY_TEXT_MVC_H
#define PSGPLAY_TEXT_MVC_H

struct text_mode;

struct text_state {
	const struct text_mode *mode;

	const char *path;
	int progress;

	int cursor;
	int track;
	enum {
		TRACK_STOP = 0,
		TRACK_PLAY,
		TRACK_PAUSE,
		TRACK_RESTART,
	} op;
	struct text_mixer {
		int volume;
	} mixer;
	u64 timestamp;
	u64 pause_offset;
	u64 pause_timestamp;

	bool redraw;
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
