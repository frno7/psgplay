// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#include <stdio.h>
#include <string.h>

#include "vt/ecma48.h"

#define ECMA48_STR(s_)		(struct vt_text) { .s = s_ }
#define ECMA48_CSI		"\033["
#define ECMA48_CSI_STR(s_)	ECMA48_STR(ECMA48_CSI s_)
#define ECMA48_SGR(n_)		ECMA48_CSI #n_ "m"
#define ECMA48_SGR_STR(n_)	ECMA48_STR(ECMA48_SGR(n_))

#define ECMA48_DEESCAPE(d)						\
	d(CUU, ECMA48_CSI "A", "\xE2\x86\x91")	/* Up */		\
	d(CUD, ECMA48_CSI "B", "\xE2\x86\x93")	/* Down */		\
	d(CUF, ECMA48_CSI "C", "\xE2\x86\x92")	/* Forward */		\
	d(CUB, ECMA48_CSI "D", "\xE2\x86\x90")	/* Backward */

/* Proper ECMA-48 */
static struct vt_text ecma48_clear(void)   { return ECMA48_CSI_STR("2J"); }
static struct vt_text ecma48_normal(void)  { return ECMA48_SGR_STR(27); };
static struct vt_text ecma48_reset(void)   { return ECMA48_SGR_STR(0); };
static struct vt_text ecma48_reverse(void) { return ECMA48_SGR_STR(7); };

/* VT320 extensions */
static struct vt_text vt320_hide(void)     { return ECMA48_CSI_STR("?25l"); };
static struct vt_text vt320_show(void)     { return ECMA48_CSI_STR("?25h"); };

static struct vt_text ecma48_position(int row, int col)
{
	struct vt_text t;

	t.s = NULL;
	snprintf(t.b, sizeof(t.b), ECMA48_CSI "%d;%dH", row + 1, col + 1);

	return t;
};

static struct vt_text ecma48_deescape(const char *s)
{
#define ECMA48_DEESCAPE_S(label_, escape_, utf8_)			\
	if (strcmp(s, escape_) == 0)					\
		return ECMA48_STR(utf8_);
ECMA48_DEESCAPE(ECMA48_DEESCAPE_S)

	return ECMA48_STR("");
}

const struct vt_cmd ecma48 = {
	.clear    = ecma48_clear,
	.hide     = vt320_hide,
	.normal   = ecma48_normal,
	.position = ecma48_position,
	.reset    = ecma48_reset,
	.reverse  = ecma48_reverse,
	.show     = vt320_show,
	.deescape = ecma48_deescape
};
