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

#include "psgplay/print.h"
#include "psgplay/sndh.h"

#include "text/main.h"
#include "text/model.h"
#include "text/view.h"

#include "system/clock.h"

#include "system/unix/poll-fifo.h"
#include "system/unix/text-mode.h"
#include "system/unix/tty.h"

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

	struct text_model tm = {
		.ctrl = {
			.track = options->track,
			.state = TRACK_PLAY,
		},
		.sndh = {
			.path = file.path,
			.size = file.size,
			.data = file.data,
		},
	};

	if (!sndh_tag_title(tm.title, sizeof(tm.title), file.data, file.size)) {
		strncpy(tm.title, file_basename(file.path), sizeof(tm.title) - 1);
		tm.title[sizeof(tm.title) - 1] = '\0';
	}

	if (!sndh_tag_subtune_count(&tm.subtune_count, file.data, file.size))
		tm.subtune_count = 1;

	clock_init();
	tty_init(&tty_events);

	const struct text_view *tv = &text_view_main;

	tty_resize_vt(&vt.vtb, tty_size());

	if (atexit(atexit_) != 0)
		pr_warn_errno("atexit");

	tv->init(&vt.vtb, &tm);

	file_nonblocking(STDIN_FILENO);
	file_nonblocking(STDOUT_FILENO);

	bool quit = false;

	for (;;) {
		vt_write_fifo(&vt.vtb, &tty_out.fifo);

		const struct poll_fifo pfs[] = {
			{ .fd = STDIN_FILENO,  .in  = &tty_in.fifo  },
			{ .fd = STDOUT_FILENO, .out = &tty_out.fifo },
		};

		if (fifo_empty(&tty_out.fifo) && quit) {
			dprintf(STDOUT_FILENO, "%s%s\n",
				vt_text(vt_reset(&vt.vtb)),
				vt_text(vt_cursor_end(&vt.vtb)));
			break;
		}

		poll_fifo(pfs, ARRAY_SIZE(pfs), clock_poll());

		clock_update();

		clock_request_ms(vt_event(&vt.vtb, clock_ms()));

		vt_deescape_fifo(&vt.vtb, &utf32_in.fifo,
			&tty_in.fifo, clock_ms());

		if (!quit)
			quit = !tv->event(&vt.vtb, &utf32_in, &tm);
	}

	atexit_();
}
