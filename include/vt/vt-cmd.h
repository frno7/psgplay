// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#ifndef VT_CMD_H
#define VT_CMD_H

/* FIXME */
struct vt_text {
	const char *s;
	char b[16];
};

/**
 * struct vt_cmd - virtual terminal command
 * @clear: clear entire screen
 * @hide: cursor hide
 * @normal: normal foreground and background colours (nonreverse)
 * @position: cursor position
 * @reset: all attributes off
 * @reverse: exchange foreground and background colours
 * @show: cursor show
 * @deescape: convert escape sequences to characters
 */
struct vt_cmd {
	struct vt_text (*clear)(void);
	struct vt_text (*hide)(void);
	struct vt_text (*normal)(void);
	struct vt_text (*position)(int row, int col);
	struct vt_text (*reset)(void);
	struct vt_text (*reverse)(void);
	struct vt_text (*show)(void);
	struct vt_text (*deescape)(const char *s);
};

#define vt_text(t) (t.s ? t.s : t.b)

struct vt_text vt_text_combine(const struct vt_text a, const struct vt_text b);

#endif /* VT_CMD_H */
