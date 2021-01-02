// SPDX-License-Identifier: GPL-2.0

#ifndef INTERNAL_STRUCT_H
#define INTERNAL_STRUCT_H

#include "internal/build-assert.h"

#define sizeof_member(type, member) sizeof(((type *)0)->member)

#define BE_STORAGE __attribute__(( __scalar_storage_order__("big-endian") ))
#define LE_STORAGE __attribute__(( __scalar_storage_order__("little-endian") ))

/* Macro definitions from the Linux kernel. */

/**
 * container_of - cast a member of a structure out to the containing structure
 * @ptr: the pointer to the member
 * @type: the type of the container struct this is embedded in
 * @member: the name of the member within the struct
 */
#define container_of(ptr, type, member) ({				\
	void *__mptr = (void *)(ptr);					\
	BUILD_BUG_ON_MSG(!__same_type(*(ptr), ((type *)0)->member) &&	\
			 !__same_type(*(ptr), void),			\
			 "pointer type mismatch in container_of()");	\
	((type *)(__mptr - offsetof(type, member))); })

#endif /* INTERNAL_STRUCT_H */
