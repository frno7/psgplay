// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <tos/bios.h>
#include <tos/gemdos.h>
#include <tos/xbios.h>

#include "internal/check-compiler.h"
#include "internal/compare.h"

#include "psgplay/sndh.h"

#include "text/mode.h"
#include "text/mvc.h"

#include "vt/vt52.h"

#include "system/atari/clock.h"
#include "system/atari/model.h"
#include "system/atari/option.h"
#include "system/atari/psg.h"
#include "system/atari/timer.h"

const char *progname = "PSGPLAY";

bool timer_c(uint32_t vector, struct xbra_regs *regs, void *arg)
{
	struct text_sndh *sndh = arg;

	model_timer(sndh);

	clock_tick();

	return true;
}

static unicode_t read_key(void)
{
	if (!gemdos_cconis())
		return 0;

	const u32 k = gemdos_cnecin();

	switch ((k >> 16) & 0xff) {
	case 0x4b: return U_ARROW_LEFT;
	case 0x48: return U_ARROW_UP;
	case 0x4d: return U_ARROW_RIGHT;
	case 0x50: return U_ARROW_DOWN;
	}

	return k & 0xff;
}

static void write_vt(struct fifo *f)
{
	for (;;) {
		char s[256];
		const size_t r = fifo_read(f, s, sizeof(s) - 1);

		if (!r)
			break;

		s[r] = '\0';
		gemdos_cconws(s);
	}
}

int main(int argc, char *argv[])
{
	struct options *options = parse_options(argc, argv);

	static DEFINE_FIFO(scr_out, 1024);
	static DEFINE_VT(scr_vt, 40, 25, &vt52);

	struct text_state model = { .path = options->input };
	struct text_state scr_view = { };
	struct text_state ctrl = { };
	struct text_sndh sndh = { };

	psg_init();
	timer_init(timer_c, &sndh);

	model_update(&model, &ctrl, &sndh, clock_ms());

	while (!model.quit) {
		vt_write_fifo(&scr_vt.vtb, &scr_out.fifo);

		const unicode_t key = read_key();

		ctrl = model;
		if (model.mode->ctrl)
			model.mode->ctrl(key, &ctrl, &model, &sndh);

		model_update(&model, &ctrl, &sndh, clock_ms());

		if (model.mode->view)
			model.mode->view(&scr_vt.vtb, &scr_view, &model, &sndh, clock_ms());

		write_vt(&scr_out.fifo);
	}

	dprintf(STDOUT_FILENO, "%s\r\n", vt_text(vt_reset(&scr_vt.vtb)));

	return EXIT_SUCCESS;
}
