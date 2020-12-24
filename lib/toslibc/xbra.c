// SPDX-License-Identifier: LGPL-2.1
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#include <tos/bios.h>
#include <tos/xbios.h>
#include <tos/xbra.h>

#include "internal/build-assert.h"
#include "internal/macro.h"
#include "internal/types.h"

struct xbra {
	const char id[4];
	const char cookie[4];
	void (*f)(void);
};

struct xbra_arg {
	void * const f;
	const uint32_t vector;
	const char *cookie;
};

static bool xbra_id_match(const struct xbra *xbra)
{
	return xbra->id[0] == 'X' &&
	       xbra->id[1] == 'B' &&
	       xbra->id[2] == 'R' &&
	       xbra->id[3] == 'A';
}

static bool xbra_cookie_match(const char *cookie, const struct xbra *xbra)
{
	return xbra->cookie[0] == cookie[0] &&
	       xbra->cookie[1] == cookie[1] &&
	       xbra->cookie[2] == cookie[2] &&
	       xbra->cookie[3] == cookie[3];
}

static struct xbra *xbra_from_f(void *f)
{
	return &((struct xbra *)f)[-1];
}

static struct xbra *xbra_next(struct xbra *xbra)
{
	xbra = xbra_from_f(xbra->f);

	return xbra_id_match(xbra) ? xbra : NULL;
}

static void ___xbra_exit(void *arg)
{
	struct xbra_arg *xbra_arg = arg;
	struct xbra *xbra = xbra_from_f(xbra_arg->f);
	const uint32_t vector = xbra_arg->vector;
	const char *cookie = xbra_arg->cookie;

	if (!xbra_id_match(xbra))
		return;

	if (xbra_cookie_match(cookie, xbra)) {
		bios_setexc(vector, xbra->f);
		return;
	}

	for (struct xbra *next = xbra_next(xbra); next != NULL;
	     ({ xbra = next; next = xbra_next(next); }))
		if (xbra_cookie_match(cookie, next)) {
			xbra->f = next->f;
			return;
		}
}

void __xbra_exit(uint32_t vector, const char cookie[5])
{
	BUILD_BUG_ON(sizeof(struct xbra) != 12);

	struct xbra_arg xbra_arg = {
		.f = bios_setexc(vector, BIOS_SETEXC_INQUIRE),
		.vector = vector,
		.cookie = cookie,
	};

	xbios_supexecarg(___xbra_exit, &xbra_arg);
}
