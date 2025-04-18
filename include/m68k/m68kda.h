// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2021 Fredrik Noring
 */

#ifndef M68KDA_H
#define M68KDA_H

#include <stddef.h>
#include <stdint.h>

#include "internal/struct.h"
#include "internal/types.h"

/**
 * struct m68kda_code - machine code major in bits 15:12
 * @major: major operation code
 */
struct m68kda_code {
	uint16_t major : 4;
	uint16_t : 12;
} BE_STORAGE;

/**
 * struct m68kda_ea - effective address operation
 * @m: mode
 * @r: register
 */
struct m68kda_ea {
	uint8_t : 2;
	uint8_t m : 3;
	uint8_t r : 3;
} BE_STORAGE;

/**
 * struct m68kda_s_ea - single effective address operation in bits 5:0
 * @m: mode
 * @r: register
 */
struct m68kda_s_ea {
	uint16_t : 10;
	uint16_t m : 3;
	uint16_t r : 3;
} BE_STORAGE;

/**
 * struct m68kda_d_ea - effective address destination operation in bits 11:6
 * @r: register
 * @m: mode
 */
struct m68kda_d_ea {
	uint16_t : 4;
	uint16_t r : 3;
	uint16_t m : 3;
	uint16_t : 6;
} BE_STORAGE;

/**
 * struct m68kda_imm3 - 3-bit immediate data word format in bits 11:9
 * @d: data 1..8 where 8 is encoded as 0
 */
struct m68kda_imm3 {
	uint16_t : 4;
	uint16_t d : 3;
	uint16_t : 9;
} BE_STORAGE;

/**
 * struct m68kda_imm4 - 4-bit immediate data word format in bits 3:0
 * @d: data 0..15
 */
struct m68kda_imm4 {
	uint16_t : 12;
	uint16_t d : 4;
} BE_STORAGE;

/**
 * struct m68kda_imm8 - 8-bit immediate data word format in bits 7:0
 * @d: data -128..127
 */
struct m68kda_imm8 {
	uint16_t : 8;
	uint16_t d : 8;
} BE_STORAGE;

/**
 * struct m68kda_sreg - register placed in bits 2:0
 * @r: register
 */
struct m68kda_sreg {
	uint16_t : 13;
	uint16_t r : 3;
} BE_STORAGE;

/**
 * struct m68kda_dreg - register placed in bits 11:9
 * @r: register
 */
struct m68kda_dreg {
	uint16_t : 4;
	uint16_t r : 3;
	uint16_t : 9;
} BE_STORAGE;

union m68kda_insn {
	struct m68kda_code code;
	struct m68kda_s_ea s_ea;
	struct m68kda_d_ea d_ea;
	struct m68kda_imm3 imm3;
	struct m68kda_imm4 imm4;
	struct m68kda_imm8 imm8;
	struct m68kda_sreg sreg;
	struct m68kda_dreg dreg;
	uint16_t word;
	uint8_t byte[2];
} BE_STORAGE;

struct m68kda;

/**
 * struct m68kda_brief_ext - brief extension word
 * @a: index register type where 0 = Dn and 1 = An
 * @r: index register
 * @l: index size where 0 = sign-extended word and 1 = long word
 * @z: ought to be zero
 * @d: sign-extended displacement
 */
struct m68kda_brief_ext {
	uint8_t a : 1;
	uint8_t r : 3;
	uint8_t l : 1;
	uint8_t z : 3;
	int8_t d;
} BE_STORAGE;

union m68kda_opdata {
	struct m68kda_brief_ext brief;
	uint32_t long_;
	uint16_t word[2];
	uint8_t byte[4];
} BE_STORAGE;

/**
 * struct m68kda_elements - instruction disassembly elements
 * @insn: instruction
 * @op:   operand
 * @d:    data register
 * @a:    address register
 * @ai:   address register indirect
 * @pi:   address register indirect with postincrement
 * @pd:   address register indirect with predecrement
 * @di:   address register indirect with displacement
 * @ix:   address register indirect with index
 * @aw:   absolute word address
 * @al:   absolute long address
 * @pcdi: program counter relative with displacement
 * @pcix: program counter relative with index
 * @bra:  branch program counter relative with displacement
 * @das:  data and address register sets
 * @imm:  immediate
 * @ccr:  condition code register
 * @sr:   status register
 * @usp:  user stack pointer register
 */
struct m68kda_elements {
	void (*insn)(struct m68kda *da);
	void (*op)(int n, struct m68kda *da);
	void (*d)(uint8_t d, struct m68kda *da);
	void (*a)(uint8_t a, struct m68kda *da);
	void (*ai)(uint8_t a, struct m68kda *da);
	void (*pi)(uint8_t a, struct m68kda *da);
	void (*pd)(uint8_t a, struct m68kda *da);
	void (*di)(int16_t displacement, uint8_t a, struct m68kda *da);
	void (*ix)(uint8_t a, struct m68kda_brief_ext brief, struct m68kda *da);
	void (*aw)(int16_t address, struct m68kda *da);
	void (*al)(int32_t address, struct m68kda *da);
	void (*pcdi)(int16_t displacement, struct m68kda *da);
	void (*pcix)(struct m68kda_brief_ext brief, struct m68kda *da);
	void (*bra)(int16_t displacement, struct m68kda *da);
	void (*das)(uint8_t ds, uint8_t as, struct m68kda *da);
	void (*imm)(int32_t n, struct m68kda *da);
	void (*ccr)(struct m68kda *da);
	void (*sr)(struct m68kda *da);
	void (*usp)(struct m68kda *da);
};

/**
 * struct m68kda_opcp - operand category and placement
 * @c: category
 * @p: placement
 */
struct m68kda_opcp {
	char c;
	char p;
};

/**
 * struct m68kda_spec - structure holding information for an opcode
 * @mnemonic: name of the instruction
 * @code: operation word code bit pattern
 * @mask: operation word mask bit pattern
 * @op0: first operand, if any
 * @op0.size: extra words of first operand, in bytes
 * @op0.opcp: first operand category and placement, or NUL
 * @op1: second operand, if any
 * @op1.size: extra words of second operand, in bytes
 * @op1.opcp: first operand category and placement, or NUL
 */
struct m68kda_spec
{
	const char *mnemonic;
	uint16_t code;
	uint16_t mask;
	struct m68kda_op {
		uint8_t size;
		struct m68kda_opcp opcp;
	} op0, op1;
};

/**
 * struct m68kda - disassembly structure
 * @spec: instruction specification
 * @elements: callbacks for every constituent part of the instruction,
 *	can be %NULL for default Motorola syntax
 * @format: output function when using default syntax elements
 * @arg: optional argument supplied to @format
 */
struct m68kda {
	const struct m68kda_spec *spec;

	const struct m68kda_elements *elements;
	int (*format)(void *arg, const char *fmt, ...)
		__attribute__((format(printf, 2, 3)));
	void *arg;
};

/**
 * m68kda_insn_find - find instruction specification
 * @insn: 16-bit instruction word
 *
 * Return: instruction specification, or %NULL if invalid
 */
const struct m68kda_spec *m68kda_insn_find(union m68kda_insn insn);

/**
 * m68kda_insn_size - instruction size in bytes including operands
 * @spec: instruction specification
 *
 * Return: instruction size in bytes including operands
 */
uint8_t m68kda_insn_size(const struct m68kda_spec *spec);

/**
 * m68kda_insn_disassemble - disassemble a 68000 processor instruction
 * @data: pointer to memory where the instruction is located
 * @size: size of @data, must be at least the size the instruction, which
 *	for the 68000 is 10 bytes
 * @elements: callbacks for every constituent part of the instruction,
 *	can be %NULL for default Motorola syntax
 * @format: output function when using default syntax elements,
 *	can be %NULL to ignore output
 * @arg: optional argument supplied to @format
 */
const struct m68kda_spec *m68kda_insn_disassemble(
	const void *data, size_t size,
	const struct m68kda_elements *elements,
	int (*format)(void *arg, const char *fmt, ...)
		__attribute__((format(printf, 2, 3))),
	void *arg);

#define M68KDG_INSTRUCTION_SPEC(					\
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

#define M68KDG_PREFIX_INDEX_OFFSET(prefix_, offset_, count_) { .n = offset_ },
#define M68KDG_PREFIX_INDEX_COUNT(prefix_, offset_, count_) { .n = count_ },
#define M68KDG_PREFIX_TABLE_ENTRY(prefix_, index_) { .index = index_ },

#endif /* M68KDA_H */
