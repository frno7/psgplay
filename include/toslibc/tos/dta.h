// SPDX-License-Identifier: LGPL-2.1
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#ifndef TOS_DTA_H
#define TOS_DTA_H

#include <stdint.h>

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
	uint8_t reserved[21];
	uint8_t attribute;
	uint16_t time;
	uint16_t date;
	uint32_t size;
	char name[14];
};

#endif /* TOS_DTA_H */
