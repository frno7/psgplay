// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2021 Fredrik Noring
 */

#include <errno.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "internal/assert.h"
#include "internal/macro.h"
#include "internal/types.h"

#include "m68k/m68kds.h"

static const char *data_register_symbol(uint8_t d)
{
	static const char *names[8] = {
		"d0", "d1", "d2", "d3", "d4", "d5", "d6", "d7"
	};

	BUG_ON(d > 7);

	return names[d];
}

static const char *address_register_symbol(uint8_t a)
{
	static const char *names[8] = {
		"a0", "a1", "a2", "a3", "a4", "a5", "a6", "sp"
	};

	BUG_ON(a > 7);

	return names[a];
}

static const char *brief_register_symbol(struct m68kda_brief_ext brief)
{
	return (brief.a ?
		address_register_symbol : data_register_symbol)(brief.r);
}

static const char *brief_size_symbol(struct m68kda_brief_ext brief)
{
	return brief.l ? ".l" : ".w";
}

static void format_insn(struct m68kda *da)
{
	da->fprintf_func(da->arg, "%s", da->spec->mnemonic);
}

static void format_op(int n, struct m68kda *da)
{
	da->fprintf_func(da->arg, "%s", !n ? "\t" : ",");
}

static void format_d(uint8_t d, struct m68kda *da)
{
	da->fprintf_func(da->arg, "%s", data_register_symbol(d));
}

static void format_a(uint8_t a, struct m68kda *da)
{
	da->fprintf_func(da->arg, "%s", address_register_symbol(a));
}

static void format_ai(uint8_t a, struct m68kda *da)
{
	da->fprintf_func(da->arg, "(%s)", address_register_symbol(a));
}

static void format_pi(uint8_t a, struct m68kda *da)
{
	da->fprintf_func(da->arg, "(%s)+", address_register_symbol(a));
}

static void format_pd(uint8_t a, struct m68kda *da)
{
	da->fprintf_func(da->arg, "-(%s)", address_register_symbol(a));
}

static void format_di(int16_t displacement, uint8_t a, struct m68kda *da)
{
	da->fprintf_func(da->arg, "%d(%s)",
		displacement, address_register_symbol(a));
}

static void format_ix(uint8_t a,
	struct m68kda_brief_ext brief, struct m68kda *da)
{
	if (!brief.d)
		da->fprintf_func(da->arg, "(%s,%s%s)",
			address_register_symbol(a),
			brief_register_symbol(brief),
			brief_size_symbol(brief));
	else
		da->fprintf_func(da->arg, "%d(%s,%s%s)",
			brief.d, address_register_symbol(a),
			brief_register_symbol(brief),
			brief_size_symbol(brief));
}

static void format_aw(int16_t address, struct m68kda *da)
{
	/* FIXME: da->print_address_func(address, da); */

	da->fprintf_func(da->arg, "$%x.w", address);
}

static void format_al(int32_t address, struct m68kda *da)
{
	/* FIXME: da->print_address_func(address, da); */

	da->fprintf_func(da->arg, "$%x.l", address);
}

static void format_pcdi(int16_t displacement, struct m68kda *da)
{
	/* FIXME: da->print_address_func(da->address + displacement, da); */

	da->fprintf_func(da->arg, "%d(pc)", displacement);
}

static void format_pcix(struct m68kda_brief_ext brief, struct m68kda *da)
{
	if (!brief.d)
		da->fprintf_func(da->arg, "(pc,%s%s)",
			brief_register_symbol(brief),
			brief_size_symbol(brief));
	else
		da->fprintf_func(da->arg, "%d(pc,%s%s)",
			brief.d, brief_register_symbol(brief),
			brief_size_symbol(brief));
}

static void format_bra(int16_t displacement, struct m68kda *da)
{
	da->print_address_func(da->address + 2 + displacement, da);
}

static void format_das(uint8_t ds, uint8_t as, struct m68kda *da)
{
	const uint16_t rs = (as << 8) | ds;
	bool range = false;
	size_t len = 0;
	char buf[64];

	if (!rs) {
		da->fprintf_func(da->arg, "#0");
		return;
	}

	buf[0] = '\0';

	for (uint16_t i = 0, m = 1; i < 16; i++, m <<= 1)
		if (rs & m) {
			if ((rs & (m << 1)) && range)
				continue;

			if (range)
				len += snprintf(&buf[len], sizeof(buf), "-");
			else if (rs & (m - 1))
				len += snprintf(&buf[len], sizeof(buf), "/");

			len += snprintf(&buf[len], sizeof(buf), "%s",
				(i < 8 ? data_register_symbol :
					 address_register_symbol)(i & 7));

			range = (rs & (m << 1)) != 0;
		}

	da->fprintf_func(da->arg, "%s", buf);
}

static void format_imm(int32_t n, struct m68kda *da)
{
	da->fprintf_func(da->arg, "#%" PRId32, n);
}

static void format_ccr(struct m68kda *da)
{
	da->fprintf_func(da->arg, "ccr");
}

static void format_sr(struct m68kda *da)
{
	da->fprintf_func(da->arg, "sr");
}

static void format_usp(struct m68kda *da)
{
	da->fprintf_func(da->arg, "usp");
}

struct m68kda_elements m68kds_motorola = {
	.insn = format_insn,
	.op   = format_op,
	.d    = format_d,
	.a    = format_a,
	.ai   = format_ai,
	.pi   = format_pi,
	.pd   = format_pd,
	.di   = format_di,
	.ix   = format_ix,
	.aw   = format_aw,
	.al   = format_al,
	.pcdi = format_pcdi,
	.pcix = format_pcix,
	.bra  = format_bra,
	.das  = format_das,
	.imm  = format_imm,
	.ccr  = format_ccr,
	.sr   = format_sr,
	.usp  = format_usp,
};
