// SPDX-License-Identifier: LGPL-2.1
/*
 * Copyright (C) 2021 Fredrik Noring
 */

#ifndef _FUJILIBC_TOS_VDI_CALL_H
#define _FUJILIBC_TOS_VDI_CALL_H

#define __VDI_CALL(call)						\
	call( 11, 1, V_BAR)						\
	call( 25, 1, VSF_COLOR)						\
	call(100, 0, V_OPNVWK)						\
	call(101, 0, V_CLSVWK)						\
	call(129, 0, VS_CLIP)

#endif /* _FUJILIBC_TOS_VDI_CALL_H */
