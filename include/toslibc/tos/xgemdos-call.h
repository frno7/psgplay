// SPDX-License-Identifier: LGPL-2.1
/*
 * Copyright (C) 2020 Fredrik Noring
 */

#ifndef _TOSLIBC_TOS_XGEMDOS_CALL_H
#define _TOSLIBC_TOS_XGEMDOS_CALL_H

#define XGEMDOS_CALL(call)						\
	call(0x00, void __NORETURN, reset,)				\
	call(0x73, void, vdi, struct vdi *vdi_)				\
	call(0xc8, void, aes, struct aes_pb *pb)			\
	call(0xfe, int16_t, gdos_version,)

#endif /* _TOSLIBC_TOS_XGEMDOS_CALL_H */
