// SPDX-License-Identifier: LGPL-2.1
/*
 * Copyright (C) 2021 Fredrik Noring
 */

#ifndef _TOSLIBC_TOS_VDI_H
#define _TOSLIBC_TOS_VDI_H

#include <stddef.h>
#include <stdint.h>

#include "internal/types.h"

#include "aes.h"
#include "vdi-call.h"

struct vdi_point {
	int16_t x;
	int16_t y;
};

struct vdi_bar {
	union {
		struct {
			struct vdi_point p1;
			struct vdi_point p2;
		};
		struct vdi_point point[2];
	};
};

/**
 * struct vdi_contrl - VDI control structure
 * @opc: function operation code
 * @ptsin: number of input vertices in ptsin
 * @ptsout: number of output vertices in ptsout
 * @intin: number of parameters in intin
 * @intout: number of output values in intout
 * @sub: function sub-operation code
 * @vdi_id: workstation vdi_id (when necessary)
 * @specific: function specific
 *
 * @opc, @ptsin, @intin, @sub and @vdi_id are filled in by the
 * application whereas @ptsout and @intout are filled in by the VDI.
 */
struct vdi_contrl {
	union {
		struct {
			int16_t opc;
			int16_t ptsin;
			int16_t ptsout;
			int16_t intin;
			int16_t intout;
			int16_t sub;
			int16_t vdi_id;
			int16_t specific[5];
		};
		int16_t word[12];
	};
};

struct vdi_intin {
	int16_t word[128];
};

struct vdi_ptsin {
	struct vdi_point point[128];
};

struct vdi_intout {
	int16_t word[128];
};

struct vdi_ptsout {
	struct vdi_point point[128];
};

/**
 * struct vdi_pb - VDI parameter block
 * @contrl: pointer to VDI control structure
 * @intin: pointer to intin array, or %NULL
 * @ptsin: pointer to ptsin array, or %NULL
 * @intout: pointer to intout array, or %NULL
 * @ptsout: pointer to ptsout array, or %NULL
 */
struct vdi_pb {
	struct vdi_contrl *contrl;
	const int16_t *intin;
	const struct vdi_point *ptsin;
	int16_t *intout;
	struct vdi_point *ptsout;
};

/**
 * enum vdi_cs_type - VDI coordinate system type
 * @VDI_CS_TYPE_NDC: normalized device coordinates
 * @VDI_CS_TYPE_RC: raster coordinates
 */
enum vdi_cs_type {
	VDI_CS_TYPE_NDC = 0,
	VDI_CS_TYPE_RC  = 2,
};

enum vdi_clip {
	VDI_CLIP_OFF,
	VDI_CLIP_ON
};

struct vdi_workstation_default {
	union {
		struct {
			int16_t device_id;
			int16_t line_type;
			int16_t line_color;
			int16_t marker_type;
			int16_t marker_color;
			int16_t font;
			int16_t text_color;
			int16_t fill_interior;
			int16_t fill_style;
			int16_t fill_color;
			int16_t cs_type;
		};
		int16_t word[11];
	};
};

struct vdi_workstation {
	union {
		struct {
			int16_t xres;
			int16_t yres;
			int16_t noscale;
			int16_t wpixel;
			int16_t hpixel;
			int16_t cheights;
			int16_t linetypes;
			int16_t linewidths;
			int16_t markertypes;
			int16_t markersizes;
			int16_t faces;
			int16_t patterns;
			int16_t hatches;
			int16_t colors;
			int16_t ngdps;
			int16_t cangdps[10];
			int16_t gdpattr[10];
			int16_t cancolor;
			int16_t cantextrot;
			int16_t canfillarea;
			int16_t cancellarray;
			int16_t palette;
			int16_t locators;
			int16_t valuators;
			int16_t choicedevs;
			int16_t stringdevs;
			int16_t wstype;
			int16_t minwchar;
			int16_t minhchar;
			int16_t maxwchar;
			int16_t maxhchar;
			int16_t minwline;
			int16_t zero5;
			int16_t maxwline;
			int16_t zero7;
			int16_t minwmark;
			int16_t minhmark;
			int16_t maxwmark;
			int16_t maxhmark;
		};
		int16_t word[57];
	};
};

enum {
#define __VDI_CALL_ENUM(opc_, sub_, label_)				\
	VDI_OPC_ ## label_ = opc_,					\
	VDI_SUB_ ## label_ = sub_,
__VDI_CALL(__VDI_CALL_ENUM)
};

static inline struct vdi_bar vdi_bar_from_aes(struct aes_bar bar)
{
	return (struct vdi_bar) {
		.p1 = {
			.x = bar.p.x,
			.y = bar.p.y,
		},
		.p2 = {
			.x = bar.p.x + bar.r.w - 1,
			.y = bar.p.y + bar.r.h - 1,
		},
	};
}

void vdi_v_bar(int16_t vdi_id, struct vdi_bar rect);

void vdi_v_opnvwk__(
	const struct vdi_workstation_default *wsd,
	int16_t *vdi_id,
	struct vdi_workstation *ws);

int16_t vdi_v_opnvwk(
	struct aes *aes,
	struct aes_graf_cell_box *gcb,
	const struct vdi_workstation_default *wsd,
	struct vdi_workstation *ws);

void vdi_v_clsvwk(int16_t vdi_id);

void vdi_vs_clip_on(int16_t vdi_id, struct vdi_bar bar);

void vdi_vs_clip_off(int16_t vdi_id);

int16_t vdi_vsf_color(int16_t vdi_id, int16_t color);

#endif /* _TOSLIBC_TOS_VDI_H */
