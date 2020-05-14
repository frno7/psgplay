// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "vt/ecma48.h"
#include "vt/vt.h"

#include "psgplay/assert.h"
#include "psgplay/print.h"
#include "psgplay/psgplay.h"
#include "psgplay/sndh.h"

#include "text/main.h"
#include "text/mode.h"
#include "text/mvc.h"

#include "system/clock.h"

#include "system/unix/poll-fifo.h"
#include "system/unix/text-mode.h"
#include "system/unix/tty.h"

struct sample_buffer {
	u64 timestamp;

	size_t size;
	size_t index;
	struct psgplay_stereo buffer[4096];

	struct psgplay *pp;

	const struct output *output;
	void *output_arg;
};

static struct sample_buffer sample_buffer_init(const void *data, size_t size,
	int track, int frequency, const struct output *output)
{
	struct sample_buffer sb = {
		.pp = psgplay_init(data, size, track, frequency),
		.output = output,
		.output_arg = output->open(NULL, frequency, true),
	};

	if (!sb.pp)
		pr_fatal_error("Failed to init PSG play\n");

	return sb;
}

static u64 sample_buffer_update(struct sample_buffer *sb, u64 timestamp)
{
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
			sb->timestamp = timestamp + 100;
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
	struct vt_buffer *vtb = arg;

	tty_resize_vt(vtb, size);

	vt_redraw(vtb);
}

static void tty_suspend(void *arg)
{
	struct vt_buffer *vtb = arg;

	dprintf(STDOUT_FILENO, "%s%s\n",
		vt_text(vt_reset(vtb)),
		vt_text(vt_cursor_end(vtb)));
}

static void tty_resume(void *arg)
{
	struct vt_buffer *vtb = arg;

	vt_redraw(vtb);
}

void text_replay(const struct options *options, struct file file,
	const struct output *output, const struct machine *machine)
{
	DEFINE_FIFO(tty_in, 256);
	DEFINE_FIFO(tty_out, 4096);
	DEFINE_FIFO_UTF32(utf32_in);
	DEFINE_VT(vt, 40, 25, &ecma48);

	struct tty_events tty_events = {
		.resize  = tty_resize,
		.suspend = tty_suspend,
		.resume  = tty_resume,
		.arg = &vt.vtb
	};

	struct text_state model = {
		.cursor = options->track,
		.track = options->track,
		.op = TRACK_PLAY,
	};
	struct text_state view = { };
	struct text_state ctrl = { };

	struct text_sndh sndh = {
		.path = file.path,
		.size = file.size,
		.data = file.data,
	};

	if (!sndh_tag_title(sndh.title, sizeof(sndh.title), file.data, file.size)) {
		strncpy(sndh.title, file_basename(file.path), sizeof(sndh.title) - 1);
		sndh.title[sizeof(sndh.title) - 1] = '\0';
	}

	if (!sndh_tag_subtune_count(&sndh.subtune_count, file.data, file.size))
		sndh.subtune_count = 1;

	struct sample_buffer sb = sample_buffer_init(file.data, file.size,
		options->track, options->frequency, output);

	clock_init();
	clock_request_ms(1);

	tty_init(&tty_events);

	const struct text_mode *tm = &text_mode_main;

	tty_resize_vt(&vt.vtb, tty_size());

	if (atexit(atexit_) != 0)
		pr_warn_errno("atexit");

	file_nonblocking(STDIN_FILENO);
	file_nonblocking(STDOUT_FILENO);

	for (;;) {
		vt_write_fifo(&vt.vtb, &tty_out.fifo);

		const struct poll_fifo pfs[] = {
			{ .fd = STDIN_FILENO,  .in  = &tty_in.fifo  },
			{ .fd = STDOUT_FILENO, .out = &tty_out.fifo },
		};

		if (fifo_empty(&tty_out.fifo) && model.quit) {
			dprintf(STDOUT_FILENO, "%s%s\n",
				vt_text(vt_reset(&vt.vtb)),
				vt_text(vt_cursor_end(&vt.vtb)));
			break;
		}

		const unicode_t key = fifo_utf32(&utf32_in);

		poll_fifo(pfs, ARRAY_SIZE(pfs), key ? 0 : clock_poll());

		clock_update();

		clock_request_ms(vt_event(&vt.vtb, clock_ms()));
		vt_deescape_fifo(&vt.vtb, &utf32_in.fifo,
			&tty_in.fifo, clock_ms());

		if (key && tm->ctrl) {
			ctrl = model;

			tm->ctrl(key, &ctrl, &model, &sndh);
		}

		if (ctrl.quit)
			model.quit = true;
		if (1 <= ctrl.cursor && ctrl.cursor <= sndh.subtune_count)
			model.cursor = ctrl.cursor;

		clock_request_ms(sample_buffer_update(&sb, clock_ms()));

		if (tm->view)
			tm->view(&vt.vtb, &view, &model, &sndh);
	}

	sample_buffer_exit(&sb);

	atexit_();
}
