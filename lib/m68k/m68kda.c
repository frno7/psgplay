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

#include "m68k/m68kda.h"
#include "m68k/m68kdg.h"
#include "m68k/m68kds.h"

static const struct m68kda_spec m68kda_opcodes[] =
{
#define M68KDG_INSTRUCTION_ENTRY(					\
		mnemonic_, code_, mask_,				\
		size0_, opc0_, opp0_,					\
		size1_, opc1_, opp1_)					\
	{								\
		.mnemonic = mnemonic_,					\
		.code = code_,						\
		.mask = mask_,						\
		.op0 = {						\
			.size = size0_,					\
			.opcp = {					\
				.c = opc0_,				\
				.p = opp0_,				\
			},						\
		},							\
		.op1 = {						\
			.size = size1_,					\
			.opcp = {					\
				.c = opc1_,				\
				.p = opp1_,				\
			},						\
		},							\
	},
M68KDG_INSTRUCTIONS(M68KDG_INSTRUCTION_ENTRY)
};

/* Reverse 16 bits. */
static uint16_t rev16(uint16_t w)
{
	uint16_t m = 0;

	for (int i = 0; i < 16; i++, w >>= 1)
		m |= (w & 1) << (15 - i);

	return m;
}

static void op_das(uint16_t das, struct m68kda *da)
{
	da->operands->das(das & 0xff, (das >> 8) & 0xff, da);
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
	case 'b': return da->operands->imm(op_byte(opdata), da);
	case 'w': return da->operands->imm(op_word(opdata), da);
	case 'l': return da->operands->imm(op_long(opdata), da);
	default: BUG();
	}
}

static void op_ea7(const struct m68kda_opcp opcp,
	const struct m68kda_ea ea, const union m68kda_opdata opdata,
	struct m68kda *da)
{
	switch (ea.r) {
	case 0: return da->operands->aw(op_word(opdata), da);
	case 1: return da->operands->al(op_long(opdata), da);
	case 2: return da->operands->pcdi(op_word(opdata), da);
	case 3: return da->operands->pcix(op_brief(opdata), da);
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
	case 0: return da->operands->d(ea.r, da);
	case 1: return da->operands->a(ea.r, da);
	case 2: return da->operands->ai(ea.r, da);
	case 3: return da->operands->pi(ea.r, da);
	case 4: return da->operands->pd(ea.r, da);
	case 5: return da->operands->di(op_word(opdata), ea.r, da);
	case 6: return da->operands->ix(ea.r, op_brief(opdata), da);
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
	case 'B': return da->operands->bra(op_imm8(insn), da);
	case 'w': return da->operands->bra(op_word(opdata), da);
	default: BUG();
	}
}

static void print_insn_arg(const struct m68kda_opcp opcp,
	const union m68kda_insn insn, const union m68kda_opdata opdata,
	struct m68kda *da)
{
	switch (opcp.c) {
	case 'C': return da->operands->ccr(da);
	case 'S': return da->operands->sr(da);
	case 'U': return da->operands->usp(da);
	case 'Q': return da->operands->imm(op_imm3(insn), da);
	case 'M': return da->operands->imm(op_imm8(insn), da);
	case 'T': return da->operands->imm(op_imm4(insn), da);
	case 'D': return da->operands->d(op_reg(opcp, insn), da);
	case 'A': return da->operands->a(op_reg(opcp, insn), da);
	case 'd': return da->operands->di(op_word(opdata), op_sreg(insn), da);
	case '+': return da->operands->pi(op_reg(opcp, insn), da);
	case '-': return da->operands->pd(op_reg(opcp, insn), da);
	case '#': return op_imm(opcp, opdata, da);
	case 'B': return op_bra(opcp, insn, opdata, da);
	case '@': return op_ea(opcp, insn, opdata, da);
	case 'L': return op_das(op_word(opdata), da);
	case 'l': return op_das(rev16(op_word(opdata)), da);
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

static bool m68kda_read(void *buffer, size_t nbytes, uint32_t address,
	struct m68kda *da)
{
	uint8_t *b = buffer;

	if (da->read_memory_func(address, b, nbytes, da) != 0)
		return false;

	return true;
}

const struct m68kda_spec *m68kda_find_insn(union m68kda_insn insn)
{
	/* FIXME: Lookup table on instruction prefix */

	for (int i = 0; i < ARRAY_SIZE(m68kda_opcodes); i++) {
		const struct m68kda_spec *spec = &m68kda_opcodes[i];

		if (m68kda_opcode_match(insn, spec))
			return spec;
	}

	return NULL;
}

static int print_insn_m68k(uint32_t memaddr, struct m68kda *da)
{
	union m68kda_insn insn;

	da->address = memaddr;

	BUILD_BUG_ON(sizeof(insn) != 2);
	if (!m68kda_read(&insn, sizeof(insn), da->address, da))
		return 0;

	const struct m68kda_spec *spec = m68kda_find_insn(insn);
	if (!spec)
		return 0;	/* Zero out on undefined instructions. */

	union m68kda_opdata op0_data = { };
	union m68kda_opdata op1_data = { };

	if (spec->op0.size > 0)
		if (!m68kda_read(&op0_data, spec->op0.size,
				da->address + sizeof(insn), da))
			return 0;
	if (spec->op1.size > 0)
		if (!m68kda_read(&op1_data, spec->op1.size,
				da->address + sizeof(insn) +
				spec->op0.size, da))
			return 0;

	da->mnemonic = spec->mnemonic;

	da->fprintf_func(da->stream, "%s", spec->mnemonic);

	if (m68kda_opcode_count(spec) > 0) {
		da->fprintf_func(da->stream, "\t");

		print_insn_arg(spec->op0.opcp, insn, op0_data, da);
	}
	if (m68kda_opcode_count(spec) > 1) {
		da->fprintf_func(da->stream, ",");

		print_insn_arg(spec->op1.opcp, insn, op1_data, da);
	}

	return sizeof(insn) + spec->op0.size + spec->op1.size;
}

static int read_buffer(uint32_t memaddr, void *myaddr,
	int length, struct m68kda *da)
{
	const size_t offset = memaddr - da->insn_memory->address;

	if (length < 1 || da->insn_memory->size < offset + length)
		return -EFAULT;

	memcpy(myaddr, &da->insn_memory->data[offset], length);

	return 0;
}

static void memory_error(int status, uint32_t memaddr,
	struct m68kda *da)
{
#if 0
	fprintf(stderr, "%s: %d %" PRIu64 "\n",		/* FIXME */
		__func__, status, memaddr);
#endif
}

static void print_address(uint32_t addr, struct m68kda *da)
{
	struct m68kda_symbol sym = { };

	da->target = addr;

	if (da->symbol)
		sym = da->symbol(da->stream, addr);

	if (sym.s[0])
		da->fprintf_func(da->stream, "%s", sym.s);
	else
		da->fprintf_func(da->stream, "0x%" PRIx32,
			addr & 0xffffff);
}

int m68kda_disassemble_instruction(const void *data, size_t size,
	uint32_t address,
	struct m68kda_symbol (*symbol)(void *arg, uint32_t address),
	int (*print)(void *arg, const char *fmt, ...),
	void *arg)
{
	const struct insn_memory insn_memory = {
		.size = size,
		.data = data,
		.address = address,
	};
	struct m68kda da = {
		.insn_memory = &insn_memory,
		.fprintf_func = print,
		.stream = arg,

		.read_memory_func = read_buffer,
		.memory_error_func = memory_error,
		.print_address_func = print_address,
		.symbol = symbol,

		.operands = &m68kds_motorola,
	};

	return print_insn_m68k(address, &da);
}

static int ignore_print(void *stream, const char *format, ...)
{
	return 0;
}

int m68kda_disassemble_type_target(const void *data, size_t size,
	uint32_t address, const char **mnemonic, uint32_t *target)
{
	const struct insn_memory insn_memory = {
		.size = size,
		.data = data,
		.address = address,
	};
	struct m68kda da = {
		.insn_memory = &insn_memory,
		.fprintf_func = ignore_print,
		.stream = NULL,

		.read_memory_func = read_buffer,
		.memory_error_func = memory_error,
		.print_address_func = print_address,

		.operands = &m68kds_motorola,
	};

	int r = print_insn_m68k(address, &da);

	if (mnemonic)
		*mnemonic = da.mnemonic;

	if (target)
		*target = da.target;

	return r;
}
