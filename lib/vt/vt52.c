// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#include <stdio.h>
#include <string.h>

#include "vt/vt52.h"

#define VT52_STR(s_)		(struct vt_text) { .s = s_ }
#define VT52_ESC		"\033"
#define VT52_ESC_STR(s_)	VT52_STR(VT52_ESC s_)

/* Proper VT52 */
static struct vt_text vt52_clear(void)
{
	return VT52_STR(VT52_ESC "H" VT52_ESC "J");
}

static struct vt_text vt52_position(int row, int col)
{
	struct vt_text t;

	t.s = NULL;
	snprintf(t.b, sizeof(t.b), VT52_ESC "Y%c%c", row + 32, col + 32);

	return t;
};

/* Atari ST extensions */
static struct vt_text vt52_reset(void)   { return VT52_ESC_STR("q"); };
static struct vt_text vt52_normal(void)  { return VT52_ESC_STR("q"); };
static struct vt_text vt52_reverse(void) { return VT52_ESC_STR("p"); };
static struct vt_text vt52_hide(void)    { return VT52_ESC_STR("f"); };
static struct vt_text vt52_show(void)    { return VT52_ESC_STR("e"); };

const struct vt_cmd vt52 = {
	.clear    = vt52_clear,
	.hide     = vt52_hide,
	.normal   = vt52_normal,
	.position = vt52_position,
	.reset    = vt52_reset,
	.reverse  = vt52_reverse,
	.show     = vt52_show,
};
