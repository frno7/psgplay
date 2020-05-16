// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#ifndef TOS_BASEPAGE_H
#define TOS_BASEPAGE_H

#include <stdint.h>

#include "dta.h"

struct _tpa {
	void *lo;	/* Bottom of Transient Program Area (TPA) */
	void *hi;	/* Top of Transient Program Area (TPA) */
};

struct _segment {
	void *base;	/* Base address of segment */
	u32 size;	/* Size of segment in bytes */
};

struct _basepage {
	struct _tpa tpa;
	struct _segment text;
	struct _segment data;
	struct _segment bss;
	struct _dta *dta;
	struct _basepage *parent;
	void *reserved;
	char *env;
	u8 unused[8];
	u32 undefined[18];
	u8 cmdlen;
	char cmd[127];
};

extern struct _basepage *_basepage;

#endif /* TOS_BASEPAGE_H */
