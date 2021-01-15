// SPDX-License-Identifier: LGPL-2.1
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#ifndef _TOSLIBC_TOS_BIOS_CALL_H
#define _TOSLIBC_TOS_BIOS_CALL_H

#define BIOS_CALL(call)							\
call(0x0, void,    getmpb,   l,     struct _mpb *ptr)			\
call(0x1, int16_t, bconstat, w,     int16_t dev)			\
call(0x2, int32_t, bconin,   w,     int16_t dev)			\
call(0x3, int32_t, bconout,  ww,    int16_t dev, int16_t c)		\
call(0x4, int32_t, rwabs,    wlwww, int16_t rwflag, void *buffer, int16_t number, int16_t recno, int16_t dev, int32_t lrecno)\
call(0x5, void *,  setexc,   wl,    uint16_t number, void (*vector)(void))\
call(0x6, int32_t, tickcal,)						\
call(0x7, void *,  getbpb,   w,     int16_t dev)			\
call(0x8, int16_t, bcostat,  w,     int16_t dev)			\
call(0x9, int16_t, mediach,  w,     int16_t dev)			\
call(0xa, int32_t, drvmap,)						\
call(0xb, int32_t, kbshift,  w,     int16_t mode)

#endif /* _TOSLIBC_TOS_BIOS_CALL_H */
