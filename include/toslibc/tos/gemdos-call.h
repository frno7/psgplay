// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#ifndef _TOSLIBC_TOS_GEMDOS_CALL_H
#define _TOSLIBC_TOS_GEMDOS_CALL_H

#define __NORETURN void __attribute__((__noreturn__))

#define GEMDOS_CALL(call)								\
call(0x00, __NORETURN,    pterm0,)							\
call(0x01, int32_t,       cconin,)							\
call(0x02, void,          cconout,  w,    int16_t c)					\
call(0x03, int16_t,       cauxin,)							\
call(0x04, void,          cauxout,  w,    int16_t c)					\
call(0x05, int16_t,       cprnout,  w,    int16_t c)					\
call(0x06, int32_t,       crawio,   w,    int16_t c)					\
call(0x07, int32_t,       crawcin,)							\
call(0x08, int32_t,       cnecin,)							\
call(0x09, int16_t,       cconws,   l,    const char *str)				\
call(0x0a, int32_t,       cconrs,   l,    char *buf)					\
call(0x0b, int16_t,       cconis,)							\
call(0x0e, int32_t,       dsetdrv,  w,    int16_t d)					\
call(0x10, int16_t,       cconos,)							\
call(0x11, int16_t,       cprnos,)							\
call(0x12, int16_t,       cauxis,)							\
call(0x13, int16_t,       cauxos,)							\
call(0x19, int16_t,       dgetdrv,)							\
call(0x1a, void,          fsetdta,  l,    struct _dta *dta)				\
call(0x20, int32_t,       super,    l,    int32_t new_ssp)				\
call(0x2a, struct _date,  tgetdate,)							\
call(0x2b, int32_t,       tsetdate, w,    struct _date)					\
call(0x2c, struct _time,  tgettime,)							\
call(0x2d, int32_t,       tsettime, w,    struct _time)					\
call(0x2f, struct _dta *, fgetdta,)							\
call(0x30, int16_t,       sversion,)							\
call(0x31, void,          ptermres, lw,   int32_t save, int16_t code)			\
call(0x36, int32_t,       dfree,    lw,   int32_t *buf, int16_t d)			\
call(0x39, int16_t,       dcreate,  l,    const char *name)				\
call(0x3a, int32_t,       ddelete,  l,    const char *path)				\
call(0x3b, int32_t,       dsetpath, l,    const char *path)				\
call(0x3c, int32_t,       fcreate,  lw,   const char *name, uint16_t attrib)		\
call(0x3d, int32_t,       fopen,    lw,   const char *name, uint16_t mode)		\
call(0x3e, int32_t,       fclose,   w,    int16_t fh)					\
call(0x3f, int32_t,       fread,    wll,  int16_t fh, int32_t count, char *buf)		\
call(0x40, int32_t,       fwrite,   wll,  int16_t fh, int32_t count, const char *buf)	\
call(0x41, int32_t,       fdelete,  l,    const char *path)				\
call(0x42, int32_t,       fseek,    lww,  int32_t place, int16_t fh, int16_t how)	\
call(0x43, int16_t,       fattrib,  lww,  const char *name, int16_t rwflag, int16_t attr)\
call(0x44, int32_t,       mxalloc,  lw,   int32_t size, int16_t mode)			\
call(0x45, int32_t,       fdup,     w,    int16_t fh)					\
call(0x46, int32_t,       fforce,   ww,   int16_t newh, int16_t oldh)			\
call(0x47, int32_t,       dgetpath, lw,   char *path, int16_t drv)			\
call(0x48, void *,        malloc,   l,    int32_t size)					\
call(0x49, int32_t,       mfree,    l,    void *ptr)					\
call(0x4a, int32_t,       mshrink,  wll,  int16_t unused, void *block, int32_t size)	\
call(0x4b, int32_t,       pexec,    wlll, int16_t mode, const void *ptr1, const void *ptr2, const void *ptr3)\
call(0x4c, __NORETURN,    pterm,    w,    int16_t code)					\
call(0x4e, int32_t,       fsfirst,  lw,   const char *path, uint16_t attrib)		\
call(0x4f, int32_t,       fsnext,)							\
call(0x56, int16_t,       frename,  wll,  int16_t junk, const char *old, const char *new)\
call(0x57, int32_t,       fdatime,  lww,  int16_t *timeptr, int16_t fh, int16_t wflag)

#endif /* _TOSLIBC_TOS_GEMDOS_CALL_H */
