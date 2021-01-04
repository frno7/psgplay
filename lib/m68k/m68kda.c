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
#include "internal/bit.h"
#include "internal/macro.h"
#include "internal/types.h"

#include "m68k/m68kda.h"
#include "m68k/m68kdg.h"
#include "m68k/m68kds.h"

static const struct m68kda_spec m68kda_opcodes[] =
{
	M68KDG_INSTRUCTIONS(M68KDG_INSTRUCTION_SPEC)
};

static void op_das(uint16_t das, struct m68kda *da)
{
	da->elements->das(das & 0xff, (das >> 8) & 0xff, da);
}

static struct m68kda_ea eap(const struct m68kda_opcp opcp,
	const union m68kda_insn insn)
{
	return (struct m68kda_ea) {
		.m = (opcp.p == 'd' ? insn.d_ea.m : insn.s_ea.m),
		.r = (opcp.p == 'd' ? insn.d_ea.r : insn.s_ea.r),
	};
}

static int8_t op_byte(const union m68kda_opdata opdata)
{
	return opdata.byte[1];
}

static int16_t op_word(const union m68kda_opdata opdata)
{
	return opdata.word[0];
}

static int32_t op_long(const union m68kda_opdata opdata)
{
	return opdata.long_;
}

static struct m68kda_brief_ext op_brief(const union m68kda_opdata opdata)
{
	return opdata.brief;
}

static void op_imm(const struct m68kda_opcp opcp,
	const union m68kda_opdata opdata, struct m68kda *da)
{
	switch (opcp.p) {
	case 'b': return da->elements->imm(op_byte(opdata), da);
	case 'w': return da->elements->imm(op_word(opdata), da);
	case 'l': return da->elements->imm(op_long(opdata), da);
	default: BUG();
	}
}

static void op_ea7(const struct m68kda_opcp opcp,
	const struct m68kda_ea ea, const union m68kda_opdata opdata,
	struct m68kda *da)
{
	switch (ea.r) {
	case 0: return da->elements->aw(op_word(opdata), da);
	case 1: return da->elements->al(op_long(opdata), da);
	case 2: return da->elements->pcdi(op_word(opdata), da);
	case 3: return da->elements->pcix(op_brief(opdata), da);
	case 4:	return op_imm(opcp, opdata, da);
	default: BUG();
	}
}

static void op_ea(const struct m68kda_opcp opcp,
	const union m68kda_insn insn, const union m68kda_opdata opdata,
	struct m68kda *da)
{
	const struct m68kda_ea ea = eap(opcp, insn);

	switch (ea.m) {
	case 0: return da->elements->d(ea.r, da);
	case 1: return da->elements->a(ea.r, da);
	case 2: return da->elements->ai(ea.r, da);
	case 3: return da->elements->pi(ea.r, da);
	case 4: return da->elements->pd(ea.r, da);
	case 5: return da->elements->di(op_word(opdata), ea.r, da);
	case 6: return da->elements->ix(ea.r, op_brief(opdata), da);
	case 7: return op_ea7(opcp, ea, opdata, da);
	default: BUG();
	}
}

static int8_t op_imm3(const union m68kda_insn insn)
{
	return insn.imm3.d ? insn.imm3.d : 8;
}

static int8_t op_imm4(const union m68kda_insn insn) { return insn.imm4.d; }
static int8_t op_imm8(const union m68kda_insn insn) { return insn.imm8.d; }
static int8_t op_sreg(const union m68kda_insn insn) { return insn.sreg.r; }
static int8_t op_dreg(const union m68kda_insn insn) { return insn.dreg.r; }

static int8_t op_reg(const struct m68kda_opcp opcp,
	const union m68kda_insn insn)
{
	switch (opcp.p) {
	case 's': return op_sreg(insn);
	case 'd': return op_dreg(insn);
	default: BUG();
	}
}

static void op_bra(const struct m68kda_opcp opcp,
	const union m68kda_insn insn, const union m68kda_opdata opdata,
	struct m68kda *da)
{
	switch (opcp.p) {
	case 'B': return da->elements->bra(op_imm8(insn), da);
	case 'w': return da->elements->bra(op_word(opdata), da);
	default: BUG();
	}
}

static void print_insn_arg(const struct m68kda_opcp opcp,
	const union m68kda_insn insn, const union m68kda_opdata opdata,
	struct m68kda *da)
{
	switch (opcp.c) {
	case 'C': return da->elements->ccr(da);
	case 'S': return da->elements->sr(da);
	case 'U': return da->elements->usp(da);
	case 'Q': return da->elements->imm(op_imm3(insn), da);
	case 'M': return da->elements->imm(op_imm8(insn), da);
	case 'T': return da->elements->imm(op_imm4(insn), da);
	case 'D': return da->elements->d(op_reg(opcp, insn), da);
	case 'A': return da->elements->a(op_reg(opcp, insn), da);
	case 'd': return da->elements->di(op_word(opdata), op_sreg(insn), da);
	case '+': return da->elements->pi(op_reg(opcp, insn), da);
	case '-': return da->elements->pd(op_reg(opcp, insn), da);
	case '#': return op_imm(opcp, opdata, da);
	case 'B': return op_bra(opcp, insn, opdata, da);
	case '@': return op_ea(opcp, insn, opdata, da);
	case 'L': return op_das(op_word(opdata), da);
	case 'l': return op_das(bitrev16(op_word(opdata)), da);
	default: BUG();
	}
}

static bool m68kda_opcode_match(const union m68kda_insn insn,
	const struct m68kda_spec *spec)
{
	return (insn.word & spec->mask) == spec->code;
}

static int m68kda_opcode_count(const struct m68kda_spec *spec)
{
	return spec->op1.opcp.c ? 2 :
	       spec->op0.opcp.c ? 1 : 0;
}

const struct m68kda_spec *m68kda_insn_find(union m68kda_insn insn)
{
	/* FIXME: Lookup table on instruction prefix */

	for (int i = 0; i < ARRAY_SIZE(m68kda_opcodes); i++) {
		const struct m68kda_spec *spec = &m68kda_opcodes[i];

		if (m68kda_opcode_match(insn, spec))
			return spec;
	}

	return NULL;
}

uint8_t m68kda_insn_size(const struct m68kda_spec *spec)
{
	BUILD_BUG_ON(sizeof(union m68kda_insn) != 2);

	return sizeof(union m68kda_insn) + spec->op0.size + spec->op1.size;
}

static void m68kda_opdata_memcpy(
	union m68kda_opdata *op0_data,
	union m68kda_opdata *op1_data,
	const uint8_t *b,
	const struct m68kda_spec *spec)
{
	/* The MOVEM operand 1 register set is placed before operand 0. */
	if (spec->op1.opcp.c == 'L') {
		memcpy(op1_data, &b[sizeof(union m68kda_insn)], spec->op1.size);
		memcpy(op0_data, &b[sizeof(union m68kda_insn) + spec->op1.size],
			spec->op0.size);
	} else {
		memcpy(op0_data, &b[sizeof(union m68kda_insn)], spec->op0.size);
		memcpy(op1_data, &b[sizeof(union m68kda_insn) + spec->op0.size],
			spec->op1.size);
	}
}

static const struct m68kda_spec *print_insn_m68k(
	const void *data, const size_t size, struct m68kda *da)
{
	const uint8_t *b = data;

	union m68kda_insn insn;
	if (size < sizeof(insn))
		return NULL;
	/* FIXME: GCC bug 98502: memcpy(&insn, b, sizeof(insn)); */
	insn.byte[0] = b[0];
	insn.byte[1] = b[1];

	const struct m68kda_spec *spec = m68kda_insn_find(insn);
	if (!spec)
		return NULL;	/* Zero out on undefined instructions. */
	if (size < m68kda_insn_size(spec))
		return NULL;

	da->spec = spec;
	da->elements->insn(da);

	union m68kda_opdata op0_data = { };
	union m68kda_opdata op1_data = { };
	m68kda_opdata_memcpy(&op0_data, &op1_data, b, spec);

	if (m68kda_opcode_count(spec) > 0) {
		da->elements->op(0, da);

		print_insn_arg(spec->op0.opcp, insn, op0_data, da);
	}
	if (m68kda_opcode_count(spec) > 1) {
		da->elements->op(1, da);

		print_insn_arg(spec->op1.opcp, insn, op1_data, da);
	}

	return spec;
}

static int ignore_format(void *arg, const char *format, ...)
{
	return 0;
}

#define DEFAULT_ELEMENT(name_) .name_ = e->name_ ? e->name_ : d->name_

const struct m68kda_spec *m68kda_insn_disassemble(
	const void *data, size_t size,
	const struct m68kda_elements *elements,
	int (*format)(void *arg, const char *fmt, ...),
	void *arg)
{
	const struct m68kda_elements *d = &m68kds_motorola;
	const struct m68kda_elements *e = elements ? elements : d;
	const struct m68kda_elements elems = {
		DEFAULT_ELEMENT(insn),
		DEFAULT_ELEMENT(op),
		DEFAULT_ELEMENT(d),
		DEFAULT_ELEMENT(a),
		DEFAULT_ELEMENT(ai),
		DEFAULT_ELEMENT(pi),
		DEFAULT_ELEMENT(pd),
		DEFAULT_ELEMENT(di),
		DEFAULT_ELEMENT(ix),
		DEFAULT_ELEMENT(aw),
		DEFAULT_ELEMENT(al),
		DEFAULT_ELEMENT(pcdi),
		DEFAULT_ELEMENT(pcix),
		DEFAULT_ELEMENT(bra),
		DEFAULT_ELEMENT(das),
		DEFAULT_ELEMENT(imm),
		DEFAULT_ELEMENT(ccr),
		DEFAULT_ELEMENT(sr),
		DEFAULT_ELEMENT(usp),
	};

	struct m68kda da = {
		.elements = &elems,
		.format = format ? format : ignore_format,
		.arg = arg,
	};

	return print_insn_m68k(data, size, &da);
}
