// SPDX-License-Identifier: LGPL-2.1
/*
 * Copyright (C) 2020 Fredrik Noring
 */

#ifndef _TOSLIBC_TOS_AES_H
#define _TOSLIBC_TOS_AES_H

#include <stddef.h>
#include <stdint.h>

#include "internal/types.h"

#include "aes-call.h"

struct aes_object;	/* FIXME */

struct aes_control {
	uint16_t opcode;
	uint16_t int_in;
	uint16_t int_out;
	uint16_t addr_in;
	uint16_t addr_out;
};

struct aes_global {
	uint16_t version;
	uint16_t app_max;
	uint16_t app_id;
	uint32_t user;
	void *rsc;
	uint32_t reserved[4];
};

#define __AES_INT_IN(n_)						\
	struct aes_int_in ## n_ { int16_t n[n_]; } u ## n_
struct aes_int_in {
	union {
		__AES_INT_IN(0);
		__AES_INT_IN(1);
		__AES_INT_IN(2);
		__AES_INT_IN(3);
		__AES_INT_IN(4);
		__AES_INT_IN(5);
		__AES_INT_IN(6);
		__AES_INT_IN(7);
		__AES_INT_IN(8);
		__AES_INT_IN(9);
		__AES_INT_IN(10);
		__AES_INT_IN(11);
		__AES_INT_IN(12);
		__AES_INT_IN(13);
		__AES_INT_IN(14);
		__AES_INT_IN(15);
		__AES_INT_IN(16);
		int16_t n[16];
	}
};

struct aes_int_out {
	int16_t n[7];
};

#define __AES_ADDR_IN(n_)						\
	struct aes_addr_in ## n_ { const void *a[n_]; } u ## n_
struct aes_addr_in {
	union {
		__AES_ADDR_IN(0);
		__AES_ADDR_IN(1);
		__AES_ADDR_IN(2);
		const void *a[2];
	}
};

struct aes_addr_out {
	void *a[1];
};

struct aes_pb {
	struct aes_control *control;
	struct aes_global *global;
	const struct aes_int_in *int_in;
	struct aes_int_out *int_out;
	const struct aes_addr_in *addr_in;
	struct aes_addr_out *addr_out;
};

struct aes {
	struct aes_pb pb;
	struct aes_control control;
	struct aes_global global;
	struct aes_int_in int_in;
	struct aes_int_out int_out;
	struct aes_addr_in addr_in;
	struct aes_addr_out addr_out;
};

enum {
	AES_FORM_ICON_NONE,
	AES_FORM_ICON_EXCLAMATION,
	AES_FORM_ICON_QUESTION,
	AES_FORM_ICON_STOP,
};

enum aes_wind_mode {
	AES_WIND_END_UPDATE,
	AES_WIND_BEG_UPDATE,
	AES_WIND_END_MCTRL,
	AES_WIND_BEG_MCTRL,
};

#define AES_GRAF_MOUSE(m)						\
	m(  0, ARROW)							\
	m(  1, TEXT_CRSR)						\
	m(  2, BUSY_BEE)						\
	m(  3, POINT_HAND)						\
	m(  4, FLAT_HAND)						\
	m(  5, THIN_CROSS)						\
	m(  6, THICK_CROSS)						\
	m(  7, OUTLN_CROSS)						\
	m(255, USER_DEF)						\
	m(256, OFF)							\
	m(257, ON)

enum aes_graf_mouse {
#define AES_GRAF_MOUSE_ENUM(id_, label_)				\
	AES_GRAF_MOUSE_ ## label_ = id_,
AES_GRAF_MOUSE(AES_GRAF_MOUSE_ENUM)
};

struct aes_point {
	int16_t x;
	int16_t y;
};

struct aes_rectangle {
	int16_t w;
	int16_t h;
};

struct aes_bar {
	union {
		struct {
			struct aes_point p;
			struct aes_rectangle r;
		};
		int16_t word[4];
	};
};

struct aes_graf_cell_box {
	struct aes_rectangle cell;
	struct aes_rectangle box;
};

struct aes_graf_mouse_user_def {
	struct aes_point hot;
	int16_t nplanes;
	int16_t fg;
	int16_t bg;
	int16_t mask[16];
	int16_t data[16];
};

#define AES_WC_MASK(m)							\
	m( 0, WC_NAME)							\
	m( 1, WC_CLOSER)						\
	m( 2, WC_FULLER)						\
	m( 3, WC_MOVER)							\
	m( 4, WC_INFO)							\
	m( 5, WC_SIZER)							\
	m( 6, WC_UPARROW)						\
	m( 7, WC_DNARROW)						\
	m( 8, WC_VSLIDE)						\
	m( 9, WC_LFARROW)						\
	m(10, WC_RTARROW)						\
	m(11, WC_HSLIDE)

enum aes_wind_create_mask {
#define AES_WC_MASK_ENUM(id_, label_)					\
	AES_ ## label_ = 1 << id_,
AES_WC_MASK(AES_WC_MASK_ENUM)
};

#define AES_WF_MODE(m)							\
	m( 4, WF_WORKXYWH)						\
	m( 5, WF_CURRXYWH)						\
	m( 6, WF_PREVXYWH)						\
	m( 7, WF_FULLXYWH)						\
	m( 8, WF_HSLIDE)						\
	m( 9, WF_VSLIDE)						\
	m(10, WF_TOP	)						\
	m(11, WF_FIRSTXYWH)						\
	m(12, WF_NEXTXYWH)						\
	m(14, WF_NEWDESK)						\
	m(15, WF_HSLSIZE)						\
	m(16, WF_VSLSIZE)						\
	m(17, WF_SCREEN)

enum aes_wf_mode {
#define AES_WF_MODE_ENUM(id_, label_)					\
	AES_ ## label_ = id_,
AES_WF_MODE(AES_WF_MODE_ENUM)
};

#define AES_MESAG_TYPE(t)						\
	t(10, MN_SELECTED)						\
	t(20, WM_REDRAW)						\
	t(21, WM_TOPPED)						\
	t(22, WM_CLOSED)						\
	t(23, WM_FULLED)						\
	t(24, WM_ARROWED)						\
	t(25, WM_HSLID)							\
	t(26, WM_VSLID)							\
	t(27, WM_SIZED)							\
	t(28, WM_MOVED)							\
	t(30, WM_UNTOPPED)						\
	t(31, WM_ONTOP)							\
	t(33, WM_BOTTOM)						\
	t(34, WM_ICONIFY)						\
	t(35, WM_UNICONIFY)						\
	t(36, WM_ALLICONIFY)						\
	t(37, WM_TOOLBAR)						\
	t(40, AC_OPEN)							\
	t(41, AC_CLOSE)

enum aes_mesag_type {
#define AES_MESAG_TYPE_ENUM(id_, label_)				\
	AES_ ## label_ = id_,
AES_MESAG_TYPE(AES_MESAG_TYPE_ENUM)
};

struct aes_mesag {
	union {
		struct {
			int16_t type;
			int16_t ap_id;
			int16_t extension;
			union {
				struct {
					int16_t id;
				} wm_closed;
				struct {
					int16_t id;
					struct aes_bar b;
				} wm_redraw, wm_sized, wm_moved;
			}
		};
		int16_t word[16];
	};
};

#define __AES_DECALARE(opcode_, name_,					\
		int_in_, int_out_, addr_in_, addr_out_, ...)		\
int16_t aes_##name_(struct aes *aes_ __VA_ARGS__);
__AES_CALL(__AES_DECALARE)

int16_t aes_appl_init(struct aes *aes_);

int16_t aes_form_alertf(struct aes *aes_,
	int16_t exit_button, const char *format, ...)
	__attribute__((format(printf, 3, 4)));

int16_t aes_graf_handle(struct aes *aes_, struct aes_graf_cell_box *gcb);

#define DEFINE_AES_WIND_GET_XYWH(symbol_, type_)			\
	static inline struct aes_bar aes_wind_get_ ## symbol_ ## xywh(	\
		struct aes *aes_, int16_t win_id)			\
	{								\
		struct aes_bar bar;					\
									\
		return aes_wind_get(aes_, win_id,			\
			AES_WF_ ## type_ ## XYWH,			\
			&bar.p.x, &bar.p.y, &bar.r.w, &bar.r.h) != 0 ?	\
				bar : (struct aes_bar) { };		\
	}

DEFINE_AES_WIND_GET_XYWH(work,  WORK)
DEFINE_AES_WIND_GET_XYWH(curr,  CURR)
DEFINE_AES_WIND_GET_XYWH(prev,  PREV)
DEFINE_AES_WIND_GET_XYWH(full,  FULL)
DEFINE_AES_WIND_GET_XYWH(first, FIRST)
DEFINE_AES_WIND_GET_XYWH(next,  NEXT)

static inline aes_last_rectangle(struct aes_rectangle r)
{
	return !r.w && !r.h;
}

#define aes_wind_for_each_xywh(bar_, aes_, win_id_)			\
	/* FIXME: typecheck win_id_ */					\
	for ((bar_) = aes_wind_get_firstxywh((aes_), (win_id));		\
	     !aes_last_rectangle(bar_.r);				\
	     (bar_) = aes_wind_get_nextxywh((aes_), (win_id)))

static inline bool aes_wind_set_currxywh(
	struct aes *aes_, int16_t win_id, struct aes_bar bar)
{
	return aes_wind_set(aes_, win_id, AES_WF_CURRXYWH,
		bar.p.x, bar.p.y, bar.r.w, bar.r.h) != 0;
}

const char *aes_mesag_type_string(const enum aes_mesag_type type);

struct aes_bar aes_bar_intersect(struct aes_bar a, struct aes_bar b);

#endif /* _TOSLIBC_TOS_AES_H */
