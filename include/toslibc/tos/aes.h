// SPDX-License-Identifier: LGPL-2.1
/*
 * Copyright (C) 2020 Fredrik Noring
 */

#ifndef _TOSLIBC_TOS_AES_H
#define _TOSLIBC_TOS_AES_H

#include <stddef.h>
#include <stdint.h>

#include "internal/types.h"

#include "aes-call.h"

struct aes_object;	/* FIXME */

struct aes_control {
	uint16_t opcode;
	uint16_t int_in;
	uint16_t int_out;
	uint16_t addr_in;
	uint16_t addr_out;
};

struct aes_global {
	uint16_t version;
	uint16_t app_max;
	uint16_t app_id;
	uint32_t user;
	void *rsc;
	uint32_t reserved[4];
};

#define __AES_INT_IN(n_)						\
	struct aes_int_in ## n_ { int16_t n[n_]; } u ## n_
struct aes_int_in {
	union {
		__AES_INT_IN(0);
		__AES_INT_IN(1);
		__AES_INT_IN(2);
		__AES_INT_IN(3);
		__AES_INT_IN(4);
		__AES_INT_IN(5);
		__AES_INT_IN(6);
		__AES_INT_IN(7);
		__AES_INT_IN(8);
		__AES_INT_IN(9);
		__AES_INT_IN(10);
		__AES_INT_IN(11);
		__AES_INT_IN(12);
		__AES_INT_IN(13);
		__AES_INT_IN(14);
		__AES_INT_IN(15);
		__AES_INT_IN(16);
		int16_t n[16];
	}
};

struct aes_int_out {
	int16_t n[7];
};

#define __AES_ADDR_IN(n_)						\
	struct aes_addr_in ## n_ { const void *a[n_]; } u ## n_
struct aes_addr_in {
	union {
		__AES_ADDR_IN(0);
		__AES_ADDR_IN(1);
		__AES_ADDR_IN(2);
		const void *a[2];
	}
};

struct aes_addr_out {
	void *a[1];
};

struct aes_bp {
	struct aes_control *control;
	struct aes_global *global;
	struct aes_int_in *int_in;
	struct aes_int_out *int_out;
	struct aes_addr_in *addr_in;
	struct aes_addr_out *addr_out;
};

struct aes {
	struct aes_bp bp;
	struct aes_control control;
	struct aes_global global;
	struct aes_int_in int_in;
	struct aes_int_out int_out;
	struct aes_addr_in addr_in;
	struct aes_addr_out addr_out;
};

#define __AES_DECALARE(opcode_, name_,					\
		int_in_, int_out_, addr_in_, addr_out_, ...)		\
int16_t aes_##name_(struct aes *aes_ __VA_ARGS__);
__AES_CALL(__AES_DECALARE)

int16_t aes_appl_init(struct aes *aes_);

#endif /* _TOSLIBC_TOS_AES_H */
