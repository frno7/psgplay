// SPDX-License-Identifier: LGPL-2.1
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#ifndef _FUJILIBC_TOS_XBIOS_CALL_H
#define _FUJILIBC_TOS_XBIOS_CALL_H

#define XBIOS_CALL(call)									\
call(0x00, void,    initmouse,   wll,       int16_t type, void *param, void *mousevec)		\
call(0x01, void *,  ssbrk,       w,         int16_t size)					\
call(0x02, void *,  physbase,)									\
call(0x03, void *,  logbase,)									\
call(0x04, int16_t, getrez,)									\
call(0x05, void,    vsetscreen,  llww,      int32_t log, int32_t phys, int16_t rez, int16_t mode)\
call(0x06, void,    setpalette,  l,         void *palptr)					\
call(0x07, int16_t, setcolor,    ww,        int16_t colornum, int16_t mixture)			\
call(0x08, int16_t, floprd,      llwwwww,   void *buf, int32_t x, int16_t d, int16_t sect, int16_t trk, int16_t side, int16_t n)\
call(0x09, int16_t, flopwr,      llwwwww,   void *buf, int32_t x, int16_t d, int16_t sect, int16_t trk, int16_t side, int16_t n)\
call(0x0a, int16_t, flopfmt,     llwwwwwlw, void *buf, int32_t x, int16_t d, int16_t spt, int16_t t, int16_t sd, int16_t i, int32_t m, int16_t v)\
call(0x0b, void,    dbmsg,       wwl,       int16_t rsrvd, int16_t msg_num, int32_t msg_arg)	\
call(0x0c, void,    midiws,      wl,        int16_t cnt, const void *buf)			\
call(0x0d, void,    mfpint,      wl,        int16_t vnum, void *vptr)				\
call(0x0e, void *,  iorec,       w,         int16_t dev)					\
call(0x0f, int32_t, rsconf,      wwwwww,    int16_t baud, int16_t flow, int16_t uc, int16_t rs, int16_t ts, int16_t sc)\
call(0x10, void *,  keytbl,      lll,       char *unshift, char *shift, char *caps)		\
call(0x11, int32_t, random,)									\
call(0x12, void,    protobt,     llww,      void *buf, int32_t serial, int16_t dsktyp, int16_t exec)\
call(0x13, int16_t, flopver,     llwwwww,   void *buf, int32_t x, int16_t d, int16_t sect, int16_t trk, int16_t sd, int16_t n)\
call(0x14, void,    scrdmp,)									\
call(0x15, int16_t, cursconf,    ww,        int16_t cmd, int16_t op)				\
call(0x16, void,    settime,     l,         uint32_t datetime)					\
call(0x17, int32_t, gettime,)									\
call(0x18, void,    bioskeys,)									\
call(0x19, void,    ikbdws,      wl,        int16_t len_minus1, int32_t ptr)			\
call(0x1a, void,    jdisint,     w,         int16_t vnum)					\
call(0x1b, void,    jenabint,    w,         int16_t vnum)					\
call(0x1c, int16_t, giaccess,    ww,        int16_t data, int16_t reg)				\
call(0x1d, void,    offgibit,    w,         int16_t ormask)					\
call(0x1e, void,    ongibit,     w,         int16_t andmask)					\
call(0x1f, void,    xbtimer,     wwwl,      int16_t timer, int16_t ctrl, int16_t data, void *vptr)\
call(0x20, void *,  dosound,     l,         const char *ptr)					\
call(0x21, int16_t, setprt,      w,         int16_t config)					\
call(0x22, void *,  kbdvbase,)									\
call(0x23, int16_t, kbrate,      ww,        int16_t delay, int16_t reprate)			\
call(0x24, void,    prtblk,      l,         void *pblkptr)					\
call(0x25, void,    vsync,)									\
call(0x26, void,    supexec,     l,         void (*func)(void))					\
call(0x27, void,    puntaes,)

#endif /* _FUJILIBC_TOS_XBIOS_CALL_H */
