// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#ifndef TOS_DTA_H
#define TOS_DTA_H

#include <stdint.h>

#include "internal/types.h"	/* FIXME */

/**
 * struct _dta - Disk Transfer Address (DTA)
 * @reserved: reserved, do not use
 * @attribute: GEMDOS attributes
 * @time: GEMDOS time
 * @date: GEMDOS date
 * @size: file size in bytes
 * @name: filename
 */
struct _dta {
	u8 reserved[21];
	u8 attribute;
	u16 time;
	u16 date;
	u32 size;
	char name[14];
};

#endif /* TOS_DTA_H */
