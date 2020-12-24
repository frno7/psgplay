// SPDX-License-Identifier: LGPL-2.1
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
	uint32_t size;	/* Size of segment in bytes */
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
	uint8_t unused[8];
	uint32_t undefined[18];
	uint8_t cmdlen;
	char cmd[127];
};

extern struct _basepage *_basepage;

#endif /* TOS_BASEPAGE_H */
