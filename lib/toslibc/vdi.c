// SPDX-License-Identifier: LGPL-2.1
/*
 * Copyright (C) 2021 Fredrik Noring
 */

#include <tos/vdi.h>
#include <tos/xbios.h>
#include <tos/xgemdos.h>

#include "internal/build-assert.h"
#include "internal/macro.h"

void vdi_v_bar(int16_t vdi_id, struct vdi_bar bar)
{
	struct vdi_contrl contrl = {
		.opc = VDI_OPC_V_BAR,
		.sub = VDI_SUB_V_BAR,
		.ptsin = __ARRAY_SIZE(bar.point),
		.vdi_id = vdi_id,
	};

	struct vdi_pb pb = {
		.contrl = &contrl,

		.ptsin = bar.point,
	};

	BUILD_BUG_ON(sizeof(bar) != sizeof(int16_t[4]));

	xgemdos_vdi(&pb);
}

void vdi_v_opnvwk__(
	const struct vdi_workstation_default *wsd,
	int16_t *vdi_id,
	struct vdi_workstation *ws)
{
	struct vdi_ptsout ptsout;
	struct vdi_contrl contrl = {
		.opc = VDI_OPC_V_OPNVWK,
		.sub = VDI_SUB_V_OPNVWK,
		.ptsout = __ARRAY_SIZE(ptsout.point),
		.intin = __ARRAY_SIZE(wsd->word),
		.intout = __ARRAY_SIZE(ws->word),
		.vdi_id = *vdi_id,
	};

	struct vdi_pb pb = {
		.contrl = &contrl,
		.ptsout = ptsout.point,
		.intin  = wsd->word,
		.intout = ws->word,
	};

	BUILD_BUG_ON(sizeof(*wsd) != sizeof(int16_t[11]));
	BUILD_BUG_ON(sizeof(*ws) != sizeof(int16_t[57]));

	xgemdos_vdi(&pb);

	*vdi_id = contrl.vdi_id;
}

int16_t vdi_v_opnvwk(
	struct aes *aes,
	struct aes_graf_cell_box *gcb,
	const struct vdi_workstation_default *wsd,
	struct vdi_workstation *ws)
{
	struct aes_graf_cell_box gcb_;
	struct vdi_workstation_default wsd_;
	struct vdi_workstation ws_;

	if (!gcb)
		gcb = &gcb_;

	if (!wsd) {
		wsd = &wsd_;

		wsd_.device_id = xbios_getrez() + 2;

		for (int i = 1; i < __ARRAY_SIZE(wsd_.word) - 1; i++)
			wsd_.word[i] = 1;

		wsd_.cs_type = VDI_CS_TYPE_RC;
	}

	if (!ws)
		ws = &ws_;

	int16_t vdi_id = aes_graf_handle(aes, gcb);

	vdi_v_opnvwk__(wsd, &vdi_id, ws);

	return vdi_id;
}

void vdi_v_clsvwk(int16_t vdi_id)
{
	struct vdi_contrl contrl = {
		.opc = VDI_OPC_V_CLSVWK,
		.sub = VDI_SUB_V_CLSVWK,
		.vdi_id = vdi_id,
	};

	struct vdi_pb pb = {
		.contrl = &contrl,
	};

	xgemdos_vdi(&pb);
}

void vdi_vs_clip_on(int16_t vdi_id, struct vdi_bar bar)
{
	const int16_t intin[] = { VDI_CLIP_ON };

	struct vdi_contrl contrl = {
		.opc = VDI_OPC_VS_CLIP,
		.ptsin = __ARRAY_SIZE(bar.point),
		.intin = __ARRAY_SIZE(intin),
		.vdi_id = vdi_id,
	};

	struct vdi_pb pb = {
		.contrl = &contrl,
		.intin = intin,
		.ptsin = bar.point,
	};

	xgemdos_vdi(&pb);
}

void vdi_vs_clip_off(int16_t vdi_id)
{
	const int16_t intint[] = { VDI_CLIP_OFF };

	struct vdi_contrl contrl = {
		.opc = VDI_OPC_VS_CLIP,
		.ptsin = 0,
		.intin = __ARRAY_SIZE(intint),
		.vdi_id = vdi_id,
	};

	struct vdi_pb pb = {
		.contrl = &contrl,
		.intin = intint,
	};

	xgemdos_vdi(&pb);
}

int16_t vdi_vsf_color(int16_t vdi_id, int16_t color)
{
	const int16_t intint[] = { color };
	int16_t intout[] = { 0 };

	struct vdi_contrl contrl = {
		.opc = VDI_OPC_VSF_COLOR,
		.intin = __ARRAY_SIZE(intint),
		.intout = __ARRAY_SIZE(intout),
		.vdi_id = vdi_id,
	};

	struct vdi_pb pb = {
		.contrl = &contrl,
		.intin = intint,
		.intout = intout,
	};

	xgemdos_vdi(&pb);

	return intout[0];
}
