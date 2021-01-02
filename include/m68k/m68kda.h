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
	uint8_t m : 3;
	uint8_t r : 3;
};

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
 * @m: mode
 * @r: register
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

/**
 * struct m68kda_opcp - operand category and placement
 * @c: category
 * @p: placement
 */
struct m68kda_opcp {
	char c;
	char p;
};

#endif /* M68KDA_H */
