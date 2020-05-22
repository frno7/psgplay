// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#include <string.h>

#include <tos/xbios.h>

#include "psgplay/sndh.h"

#include "text/load.h"
#include "text/main.h"
#include "text/mode.h"
#include "text/mvc.h"

#include "system/atari/file.h"
#include "system/atari/model.h"
#include "system/atari/psg.h"

#define TIMER_C_FREQUENCY 200
#define TIMER_C_PERIOD (1000000000 / TIMER_C_FREQUENCY)		/* ns */

static struct file file_load;
static struct file_cursor file_cursor;

static volatile bool model_play_call;
static u8 pause_iomix;

static struct {
	s32 period;	/* Timer duration in ns */
	s32 timestamp;	/* Timestamp for next event in ns */
} timer;

struct sndh_init_arg {
	struct text_sndh *sndh;
	int track;
};

void model_timer(struct text_sndh *sndh)
{
	struct sndh_file sndh_file = {
		.size = sndh->size,
		.sndh = (void *)sndh->data,
	};

	if (model_play_call) {
		while (timer.timestamp <= TIMER_C_PERIOD / 2) {
			sndh_play(&sndh_file);

			timer.timestamp += timer.period;
		}

		timer.timestamp -= TIMER_C_PERIOD;
	}
}

void sndh_init_super(void *arg)
{
	struct sndh_init_arg *sndh_init_arg = arg;
	struct sndh_file sndh_file = {
		.size = sndh_init_arg->sndh->size,
		.sndh = (void *)sndh_init_arg->sndh->data,
	};


	sndh_init(sndh_init_arg->track, &sndh_file);
}

static void sndh_exit_super(void *arg)
{
	struct text_sndh *sndh = arg;
	struct sndh_file sndh_file = {
		.size = sndh->size,
		.sndh = (void *)sndh->data,
	};

	sndh_exit(&sndh_file);
}

static int sndh_frequency(const struct text_sndh *sndh)
{
	struct sndh_timer t;

	return sndh_tag_timer(&t, sndh->data, sndh->size) &&
		t.frequency > 0 ? t.frequency : 50;
}

static void model_play(struct text_state *model, int track,
	struct text_sndh *sndh, u64 timestamp)
{
	model->track = track;
	model->op = TRACK_PLAY;
	model->timestamp = timestamp;
	model->pause_offset = 0;
	model->pause_timestamp = 0;

	struct sndh_init_arg sndh_init_arg = {
		.sndh = sndh,
		.track = model->track,
	};

	xbios_supexecarg(sndh_init_super, &sndh_init_arg);

	timer.period = 1000000000 / sndh_frequency(sndh);
	timer.timestamp = 0;
	model_play_call = true;
}

static void model_update_play(struct text_state *model, const struct text_state *ctrl,
	struct text_sndh *sndh, u64 timestamp)
{
	if (model->op == TRACK_PLAY && ctrl->op == TRACK_PAUSE) {
		model->pause_timestamp = timestamp;
		model->op = ctrl->op;

		model_play_call = false;
		pause_iomix = psg_mute();
	}

	if (model->op == TRACK_PAUSE && ctrl->op == TRACK_PLAY) {
		model->pause_offset += timestamp - model->pause_timestamp;
		model->pause_timestamp = 0;
		model->op = ctrl->op;

		psg_unmute(pause_iomix);
		model_play_call = true;
	}

	if ((model->op == TRACK_PLAY ||
	     model->op == TRACK_PAUSE) &&
	    (ctrl->op == TRACK_STOP ||
	     ctrl->op == TRACK_RESTART ||
	     model->quit)) {
		model_play_call = false;
		model->op = TRACK_STOP;

		xbios_supexecarg(sndh_exit_super, sndh);
		psg_mute();
	}

	if ((model->op == TRACK_STOP && !model->quit) &&
	    (ctrl->op == TRACK_PLAY ||
	     ctrl->op == TRACK_RESTART))
		model_play(model, ctrl->track, sndh, timestamp);

	if (ctrl->cursor)
		model->cursor = ctrl->cursor;
	if (ctrl->redraw)
		model->redraw = ctrl->redraw;
}

static int default_subtune(struct file file)
{
	int track;

	if (!sndh_tag_default_subtune(&track, file.data, file.size))
		track = 1;

	return track;
}

static bool model_update_load(struct text_state *model, const struct text_state *ctrl,
	struct text_sndh *sndh, u64 timestamp)
{
	if (model->path) {
		const char *name = file_basename(model->path);

		strncpy(sndh->title, name, sizeof(sndh->title) - 1);
		sndh->title[sizeof(sndh->title) - 1] = '\0';

		sndh_read_first(model->path, &file_load, &file_cursor);

		model->path = NULL;
		model->mode = &text_mode_load;

		model->progress = sndh_read_progress(&file_cursor);

		return true;
	} else if (!sndh_read_finished(&file_cursor)) {
		sndh_read_next(&file_cursor);

		model->progress = sndh_read_progress(&file_cursor);

		if (sndh_read_finished(&file_cursor)) {
			*sndh = text_sndh_init(file_basename(file_load.path),
				file_load.path, file_load.data, file_load.size);

			model->mode = &text_mode_main;

			const int subtune = default_subtune(file_load);

			model->cursor = subtune;

			model_play(model, subtune, sndh, timestamp);
		}

		return true;
	}

	return false;
}

void model_update(struct text_state *model, const struct text_state *ctrl,
	struct text_sndh *sndh, u64 timestamp)
{
	if (ctrl->quit)
		model->quit = true;

	if (model_update_load(model, ctrl, sndh, timestamp))
		return;

	model_update_play(model, ctrl, sndh, timestamp);
}
