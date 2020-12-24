// SPDX-License-Identifier: LGPL-2.1
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#ifndef _TOSLIBC_TOS_BIOS_CALL_H
#define _TOSLIBC_TOS_BIOS_CALL_H

#define BIOS_CALL(call)							\
call(0x0, void,   getmpb,   l,     struct _mpb *ptr)			\
call(0x1, s16,    bconstat, w,     s16 dev)				\
call(0x2, s32,    bconin,   w,     s16 dev)				\
call(0x3, s32,    bconout,  ww,    s16 dev, s16 c)			\
call(0x4, s32,    rwabs,    wlwww, s16 rwflag, void *buffer, s16 number, s16 recno, s16 dev, s32 lrecno)\
call(0x5, void *, setexc,   wl,    u16 number, void (*vector)(void))	\
call(0x6, s32,    tickcal,)						\
call(0x7, void *, getbpb,   w,     s16 dev)				\
call(0x8, s16,    bcostat,  w,     s16 dev)				\
call(0x9, s16,    mediach,  w,     s16 dev)				\
call(0xa, s32,    drvmap,)						\
call(0xb, s32,    kbshift,  w,     s16 mode)

#endif /* _TOSLIBC_TOS_BIOS_CALL_H */
