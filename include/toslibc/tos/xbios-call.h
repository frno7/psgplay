// SPDX-License-Identifier: LGPL-2.1
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#ifndef _TOSLIBC_TOS_XBIOS_CALL_H
#define _TOSLIBC_TOS_XBIOS_CALL_H

#define XBIOS_CALL(call)									\
call(0x00, void,   initmouse,   wll,       s16 type, void *param, void *mousevec)		\
call(0x01, void *, ssbrk,       w,         s16 size)						\
call(0x02, void *, physbase,)									\
call(0x03, void *, logbase,)									\
call(0x04, s16,    getrez,)									\
call(0x05, void,   vsetscreen,  llww,      s32 log, s32 phys, s16 rez, s16 mode)		\
call(0x06, void,   setpalette,  l,         void *palptr)					\
call(0x07, s16,    setcolor,    ww,        s16 colornum, s16 mixture)				\
call(0x08, s16,    floprd,      llwwwww,   void *buf, s32 x, s16 d, s16 sect, s16 trk, s16 side, s16 n)\
call(0x09, s16,    flopwr,      llwwwww,   void *buf, s32 x, s16 d, s16 sect, s16 trk, s16 side, s16 n)\
call(0x0a, s16,    flopfmt,     llwwwwwlw, void *buf, s32 x, s16 d, s16 spt, s16 t, s16 sd, s16 i, s32 m, s16 v)\
call(0x0b, void,   dbmsg,       wwl,       s16 rsrvd, s16 msg_num, s32 msg_arg)			\
call(0x0c, void,   midiws,      wl,        s16 cnt, const char *buf)				\
call(0x0d, void,   mfpint,      wl,        s16 vnum, void *vptr)				\
call(0x0e, void *, iorec,       w,         s16 dev)						\
call(0x0f, s32,    rsconf,      wwwwww,    s16 baud, s16 flow, s16 uc, s16 rs, s16 ts, s16 sc)	\
call(0x10, void *, keytbl,      lll,       char *unshift, char *shift, char *caps)		\
call(0x11, s32,    random,)									\
call(0x12, void,   protobt,     llww,      void *buf, s32 serial, s16 dsktyp, s16 exec)		\
call(0x13, s16,    flopver,     llwwwww,   void *buf, s32 x, s16 d, s16 sect, s16 trk, s16 sd, s16 n)\
call(0x14, void,   scrdmp,)									\
call(0x15, s16,    cursconf,    ww,        s16 cmd, s16 op)					\
call(0x16, void,   settime,     l,         u32 datetime)					\
call(0x17, s32,    gettime,)									\
call(0x18, void,   bioskeys,)									\
call(0x19, void,   ikbdws,      wl,        s16 len_minus1, s32 ptr)				\
call(0x1a, void,   jdisint,     w,         s16 vnum)						\
call(0x1b, void,   jenabint,    w,         s16 vnum)						\
call(0x1c, s16,    giaccess,    ww,        s16 data, s16 reg)					\
call(0x1d, void,   offgibit,    w,         s16 ormask)						\
call(0x1e, void,   ongibit,     w,         s16 andmask)						\
call(0x1f, void,   xbtimer,     wwwl,      s16 timer, s16 ctrl, s16 data, void *vptr)		\
call(0x20, void *, dosound,     l,         const char *ptr)					\
call(0x21, s16,    setprt,      w,         s16 config)						\
call(0x22, void *, kbdvbase,)									\
call(0x23, s16,    kbrate,      ww,        s16 delay, s16 reprate)				\
call(0x24, void,   prtblk,      l,         void *pblkptr)					\
call(0x25, void,   vsync,)									\
call(0x26, void,   supexec,     l,         void (*func)(void))					\
call(0x27, void,   puntaes,)

#endif /* _TOSLIBC_TOS_XBIOS_CALL_H */
