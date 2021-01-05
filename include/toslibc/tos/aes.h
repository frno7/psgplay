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

struct aes_bp {
	struct aes_control *control;
	struct aes_global *global;
	struct aes_int_in *int_in;
	struct aes_int_out *int_out;
	struct aes_addr_in *addr_in;
	struct aes_addr_out *addr_out;
};

struct aes {
	struct aes_bp bp;
	struct aes_control control;
	struct aes_global global;
	struct aes_int_in int_in;
	struct aes_int_out int_out;
	struct aes_addr_in addr_in;
	struct aes_addr_out addr_out;
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

struct aes_wind_rect {
	int16_t x;
	int16_t y;
	int16_t w;
	int16_t h;
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
					struct aes_wind_rect r;
				} wm_sized, wm_moved;
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

#endif /* _TOSLIBC_TOS_AES_H */
