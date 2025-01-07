// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "internal/assert.h"
#include "internal/print.h"

#include "vt/ecma48.h"
#include "vt/vt.h"

#include "psgplay/psgplay.h"
#include "psgplay/sndh.h"

#include "text/main.h"
#include "text/mode.h"
#include "text/mvc.h"

#include "unicode/atari.h"

#include "system/unix/clock.h"
#include "system/unix/poll-fifo.h"
#include "system/unix/text-mode.h"
#include "system/unix/tty.h"

#define BUFFER_UPDATE_TIME 20	/* 20 ms */

struct sample_buffer {
	u64 timestamp;

	size_t size;
	size_t index;
	struct psgplay_stereo buffer[4096];

	struct psgplay *pp;

	const struct output *output;
	void *output_arg;
};

struct sample_mixer {
	struct text_mixer *tm;
	psgplay_digital_to_stereo_cb cb;
	struct psgplay_digital buffer[4096];
};

struct tty_arg {
	struct vt_buffer *vtb;
	const struct text_state *model;
	struct sample_buffer *sb;
	struct sample_mixer *sm;
};

void digital_to_stereo(struct psgplay_stereo *stereo,
	const struct psgplay_digital *digital, size_t count, void *arg)
{
	struct sample_mixer *sm = arg;

	if (!sm->tm->volume) {
		sm->cb(stereo, digital, count, NULL);
		return;
	}

	for (size_t k = 0; k < count; k += ARRAY_SIZE(sm->buffer)) {
		const size_t n = min(count - k, ARRAY_SIZE(sm->buffer));

		memcpy(sm->buffer, &digital[k],
			sizeof(struct psgplay_digital[n]));

		for (size_t i = 0; i < n; i++)
			sm->buffer[i].mixer.volume.main = sm->tm->volume;

		sm->cb(&stereo[k], sm->buffer, n, NULL);
	}
}

static struct psgplay *psgplay_init__(const void *data,
	size_t size, int track, int frequency, struct sample_mixer *sm)
{
	struct psgplay *pp = psgplay_init(data, size, track, frequency);

	if (!pp)
		pr_fatal_error("Failed to init PSG play\n");

	psgplay_digital_to_stereo_callback(pp, digital_to_stereo, sm);

	return pp;
}

static struct sample_buffer sample_buffer_init(
	struct sample_mixer *sm, const void *data, size_t size,
	const char *option_output, int track, int frequency,
	const struct output *output)
{
	struct sample_buffer sb = {
		.pp = psgplay_init__(data, size, track, frequency, sm),
		.output = output,
		.output_arg = output->open(option_output, frequency, true),
	};

	return sb;
}

static void sample_buffer_flush(struct sample_buffer *sb)
{
	if (!sb->pp)
		return;

	psgplay_stop(sb->pp);

	for (;;) {
		if (sb->index == sb->size) {
			sb->index = 0;
			sb->size = psgplay_read_stereo(sb->pp,
				sb->buffer, ARRAY_SIZE(sb->buffer));

			if (!sb->size)
				break;
		}

		while (sb->index < sb->size)
			if (sb->output->sample(
					sb->buffer[sb->index].left,
					sb->buffer[sb->index].right,
					sb->output_arg))
				sb->index++;
	}

	if (sb->output->flush)
		sb->output->flush(sb->output_arg);
}

static bool sample_buffer_stop(struct sample_buffer *sb)
{
	sample_buffer_flush(sb);

	psgplay_free(sb->pp);

	sb->pp = NULL;

	sb->size = sb->index = 0;

	if (sb->output->drop)
		sb->output->drop(sb->output_arg);

	return true;
}

static bool sample_buffer_pause(struct sample_buffer *sb)
{
	if (!sb->output->pause)
		return false;

	return sb->output->pause(sb->output_arg);
}

static bool sample_buffer_resume(struct sample_buffer *sb)
{
	if (!sb->output->resume)
		return false;

	return sb->output->resume(sb->output_arg);
}

static bool sample_buffer_play(struct sample_buffer *sb,
	struct sample_mixer *sm, const void *data, size_t size,
	int track, int frequency, u64 timestamp)
{
	BUG_ON(sb->pp);

	sb->pp = psgplay_init__(data, size, track, frequency, sm);

	return true;
}

static u64 sample_buffer_update(struct sample_buffer *sb, u64 timestamp)
{
	if (!sb->pp)
		return 0;

	if (timestamp < sb->timestamp)
		return sb->timestamp;

	if (sb->index == sb->size) {
		sb->index = 0;
		sb->size = psgplay_read_stereo(sb->pp,
			sb->buffer, ARRAY_SIZE(sb->buffer));
	}

	for (sb->timestamp = timestamp; sb->index < sb->size; sb->index++)
		if (!sb->output->sample(
				sb->buffer[sb->index].left,
				sb->buffer[sb->index].right,
				sb->output_arg)) {
			sb->timestamp = timestamp + (BUFFER_UPDATE_TIME);
			break;
		}

	return sb->timestamp;
}

static void sample_buffer_exit(struct sample_buffer *sb)
{
	psgplay_free(sb->pp);

	sb->output->close(sb->output_arg);
}

static void atexit_(void)
{
	static bool done;

	if (!done)
		tty_exit();

	done = true;
}

static void tty_resize_vt(struct vt_buffer *vtb, struct tty_size size)
{
	vt_client_resize(vtb, size.rows, size.cols);
}

static void tty_resize(struct tty_size size, void *arg)
{
	struct tty_arg *tty_arg = arg;
	struct vt_buffer *vtb = tty_arg->vtb;

	tty_resize_vt(vtb, size);

	vt_redraw(vtb);
}

static void tty_suspend(void *arg)
{
	struct tty_arg *tty_arg = arg;
	struct vt_buffer *vtb = tty_arg->vtb;
	struct sample_buffer *sb = tty_arg->sb;
	const struct text_state *model = tty_arg->model;

	dprintf(STDOUT_FILENO, "%s%s\n",
		vt_text(vt_reset(vtb)),
		vt_text(vt_cursor_end(vtb)));

	if (model->op == TRACK_PLAY)
		sample_buffer_pause(sb);
}

static void tty_resume(void *arg)
{
	struct tty_arg *tty_arg = arg;
	struct vt_buffer *vtb = tty_arg->vtb;
	struct sample_buffer *sb = tty_arg->sb;
	const struct text_state *model = tty_arg->model;

	if (model->op == TRACK_PLAY)
		sample_buffer_resume(sb);

	vt_redraw(vtb);
}

static void model_restart(struct sample_buffer *sb,
	struct sample_mixer *sm, const struct options *options,
	struct text_state *model, const struct text_state *ctrl,
	const struct text_sndh *sndh, u64 timestamp)
{
	if (model->op == TRACK_PLAY && ctrl->op == TRACK_PAUSE) {
		model->pause_timestamp = timestamp;
		sample_buffer_pause(sb);
		model->op = ctrl->op;
		return;
	}

	if (model->op == TRACK_PAUSE && ctrl->op == TRACK_PLAY) {
		model->pause_offset += timestamp - model->pause_timestamp;
		model->pause_timestamp = 0;
		sample_buffer_resume(sb);
		model->op = ctrl->op;
		return;
	}

	if (!sample_buffer_stop(sb))
		return;

	model->op = TRACK_STOP;

	if (ctrl->op != TRACK_PLAY &&
	    ctrl->op != TRACK_RESTART)
		return;

	if (sample_buffer_play(sb, sm, sndh->data, sndh->size,
			ctrl->track, options->frequency, timestamp)) {
		model->track = ctrl->track;
		model->op = TRACK_PLAY;
		model->timestamp = timestamp;
		model->pause_offset = 0;
		model->pause_timestamp = 0;
	}
}

static u64 model_update(struct sample_buffer *sb,
	struct sample_mixer *sm, const struct options *options,
	struct text_state *model, const struct text_state *ctrl,
	const struct text_sndh *sndh, u64 timestamp)
{
	if (ctrl->quit)
		model->quit = true;

	model->cursor = ctrl->cursor;
	model->redraw = ctrl->redraw;
	model->mixer = ctrl->mixer;

	if (ctrl->track != model->track ||
	    ctrl->op != model->op)
		model_restart(sb, sm, options, model, ctrl, sndh, timestamp);

	return sample_buffer_update(sb, timestamp);
}

static unicode_t nonspace_charset_atari_st_to_utf32(u8 c, void *arg)
{
	return isspace(c) || iscntrl(c) ?
		c : charset_atari_st_to_utf32(c, arg);
}

void text_replay(const struct options *options, struct file file,
	const struct output *output)
{
	DEFINE_FIFO(tty_in, 256);
	DEFINE_FIFO(tty_out, 4096);
	DEFINE_FIFO_UTF32(utf32_in);
	DEFINE_VT(tty_vt, 40, 25, &ecma48);

	struct text_state model = {
		.mode = &text_mode_main,
		.cursor = options->track,
		.track = options->track,
		.op = TRACK_PLAY,
	};
	struct text_state view = { };
	struct text_state ctrl = { };

	const struct text_sndh sndh = text_sndh_init(
		file_basename(file.path), file.path, file.data, file.size);

	struct sample_mixer sm = {
		.tm = &model.mixer,
		.cb = psg_mix_option(),
	};
	struct sample_buffer sb = sample_buffer_init(&sm, file.data, file.size,
		options->output, options->track, options->frequency, output);

	struct tty_arg tty_arg = {
		.vtb = &tty_vt.vtb,
		.model = &model,
		.sb = &sb,
		.sm = &sm,
	};

	struct tty_events tty_events = {
		.resize  = tty_resize,
		.suspend = tty_suspend,
		.resume  = tty_resume,
		.arg = &tty_arg
	};

	clock_init();
	clock_request_ms(1);

	tty_init(&tty_events);

	tty_resize_vt(&tty_vt.vtb, tty_size());

	if (atexit(atexit_) != 0)
		pr_warn_errno("atexit");

	file_nonblocking(STDIN_FILENO);
	file_nonblocking(STDOUT_FILENO);

	for (;;) {
		vt_write_fifo_utf8_from_charset(
			&tty_vt.vtb, &tty_out.fifo,
			nonspace_charset_atari_st_to_utf32, NULL);

		const struct poll_fifo pfs[] = {
			{ .fd = STDIN_FILENO,  .in  = &tty_in.fifo  },
			{ .fd = STDOUT_FILENO, .out = &tty_out.fifo },
		};

		if (fifo_empty(&tty_out.fifo) && model.quit) {
			dprintf(STDOUT_FILENO, "%s%s\n",
				vt_text(vt_reset(&tty_vt.vtb)),
				vt_text(vt_cursor_end(&tty_vt.vtb)));
			break;
		}

		const unicode_t key = fifo_utf32(&utf32_in);

		poll_fifo(pfs, ARRAY_SIZE(pfs), key ? 0 : clock_poll());

		clock_update();

		clock_request_ms(vt_event(&tty_vt.vtb, clock_ms()));
		vt_deescape_fifo(&tty_vt.vtb, &utf32_in.fifo,
			&tty_in.fifo, clock_ms());

		ctrl = model;
		if (model.mode->ctrl)
			model.mode->ctrl(key, &ctrl, &model, &sndh);

		clock_request_ms(model_update(&sb, &sm,
			options, &model, &ctrl, &sndh, clock_ms()));

		if (model.mode->view)
			clock_request_ms(model.mode->view(&tty_vt.vtb,
				&view, &model, &sndh, clock_ms()));
	}

	sample_buffer_exit(&sb);

	atexit_();
}
