// SPDX-License-Identifier: LGPL-2.1
/*
 * Copyright (C) 2020 Fredrik Noring
 */

#include <stdarg.h>
#include <stdio.h>

#include <tos/aes.h>
#include <tos/xgemdos.h>

#include "internal/build-assert.h"
#include "internal/macro.h"

#define AES_CONTROL(opcode_, int_in_, int_out_, addr_in_, addr_out_)	\
	aes_->control = (struct aes_control) {				\
		.opcode  = opcode_,					\
		.int_in  = int_in_,					\
		.int_out = 1 + int_out_, /* For implicit return code. */\
		.addr_in = addr_in_,					\
		.addr_out = addr_out_,					\
	}

#define AES_INT_IN_N(n_, ...)						\
	aes_->int_in.__CONCATENATE(u, n_) =				\
		(struct __CONCATENATE(aes_int_in, n_)) {		\
			.n = { __VA_ARGS__ }				\
		}
#define AES_INT_IN(...) AES_INT_IN_N(COUNT_ARGS(__VA_ARGS__), __VA_ARGS__)

#define AES_ADDR_IN_N(n_, ...)						\
	aes_->addr_in.__CONCATENATE(u, n_) =				\
		(struct __CONCATENATE(aes_addr_in, n_)) {		\
			.a = { __VA_ARGS__ }				\
		}
#define AES_ADDR_IN(...) AES_ADDR_IN_N(COUNT_ARGS(__VA_ARGS__), __VA_ARGS__)

#define AES_INT_OUT0()
#define AES_INT_OUT1(n1)								*n1 = aes_->int_out.n[1]
#define AES_INT_OUT2(n1, n2)			AES_INT_OUT1(n1);			*n2 = aes_->int_out.n[2]
#define AES_INT_OUT3(n1, n2, n3)		AES_INT_OUT2(n1, n2);			*n3 = aes_->int_out.n[3]
#define AES_INT_OUT4(n1, n2, n3, n4)		AES_INT_OUT3(n1, n2, n3);		*n4 = aes_->int_out.n[4]
#define AES_INT_OUT5(n1, n2, n3, n4, n5)	AES_INT_OUT4(n1, n2, n3, n4);		*n5 = aes_->int_out.n[5]
#define AES_INT_OUT6(n1, n2, n3, n4, n5, n6)	AES_INT_OUT5(n1, n2, n3, n4, n5);	*n6 = aes_->int_out.n[6]
#define AES_INT_OUTN(f, ...) f(__VA_ARGS__)
#define AES_INT_OUT(...) AES_INT_OUTN(					\
	__CONCATENATE(AES_INT_OUT, COUNT_ARGS(__VA_ARGS__)), __VA_ARGS__)

#define AES_ADDR_OUT0()
#define AES_ADDR_OUT1(a0) a0 = aes_->addr_out.a[0]
#define AES_ADDR_OUTN(f, ...) f(__VA_ARGS__)
#define AES_ADDR_OUT(...) AES_ADDR_OUTN(				\
	__CONCATENATE(AES_ADDR_OUT, COUNT_ARGS(__VA_ARGS__)), __VA_ARGS__)

#define __AES_DEFINE(opcode_, name_,					\
		int_in_, int_out_, addr_in_, addr_out_, ...)		\
int16_t aes_ ## name_(struct aes *aes_ __VA_ARGS__)			\
{									\
	AES_CONTROL(opcode_,						\
		COUNT_ARGS int_in_,					\
		COUNT_ARGS int_out_,					\
		COUNT_ARGS addr_in_,					\
		COUNT_ARGS addr_out_);					\
									\
	AES_INT_IN int_in_; 						\
	AES_ADDR_IN addr_in_; 						\
									\
	xgemdos_aes(aes_);						\
									\
	AES_INT_OUT int_out_;	 					\
	AES_ADDR_OUT addr_out_;						\
									\
	return aes_->int_out.n[0];					\
}
__AES_CALL(__AES_DEFINE)

int16_t aes_appl_init(struct aes *aes_)
{
	aes_->bp = (struct aes_bp) {
		.control  = &aes_->control,
		.global   = &aes_->global,
		.int_in   = &aes_->int_in,
		.int_out  = &aes_->int_out,
		.addr_in  = &aes_->addr_in,
		.addr_out = &aes_->addr_out,
	};

	return aes_appl_init__(aes_);
}

int16_t aes_form_alertf(struct aes *aes_,
	int16_t exit_button, const char *format, ...)
{
	char buffer[256];
	va_list ap;

	va_start(ap, format);
	vsnprintf(buffer, sizeof(buffer), format, ap);
	va_end(ap);

	return aes_form_alert(aes_, exit_button, buffer);
}

int16_t aes_graf_handle(struct aes *aes_, struct aes_graf_cell_box *gcb)
{
	struct aes_graf_cell_box gcb_;

	if (!gcb)
		gcb = &gcb_;

	BUILD_BUG_ON(sizeof(*gcb) != sizeof(int16_t[4]));

	return aes_graf_handle__(aes_,
		&gcb->cell.w, &gcb->cell.h,
		&gcb->box.w, &gcb->box.h);
}

const char *aes_mesag_type_string(const enum aes_mesag_type type)
{
	switch (type) {
#define AES_MESAG_TYPE_CASE(id_, label_)				\
	case AES_ ## label_: return #label_;
AES_MESAG_TYPE(AES_MESAG_TYPE_CASE)
	default: return "";
	}
}
