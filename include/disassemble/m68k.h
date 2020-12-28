// SPDX-License-Identifier: GPL-2.0

#ifndef DISASSEMBLE_M68K_H
#define DISASSEMBLE_M68K_H

#include <stddef.h>
#include <stdint.h>

/**
 * enum m68k_insn_type - instruction type
 * @m68k_insn_inv: invalid instruction
 * @m68k_insn_ins: instruction except jump
 * @m68k_insn_jmp: unconditional jump
 * @m68k_insn_jcc: conditional jump
 * @m68k_insn_jsr: jump to subroutine
 * @m68k_insn_ret: return instruction
 */
enum m68k_insn_type {
	m68k_insn_inv,
	m68k_insn_ins,
	m68k_insn_jmp,
	m68k_insn_jcc,
	m68k_insn_jsr,
	m68k_insn_ret,
};

struct m68k_symbol {
	char s[16];
};

int m68k_disassemble_instruction(const void *data, size_t size,
	uint32_t address,
	struct m68k_symbol (*symbol)(void *arg, uint32_t address),
	int (*print)(void *arg, const char *fmt, ...),
	void *arg);

int m68k_disassemble_type_target(const void *data, size_t size,
	uint32_t address, enum m68k_insn_type *type, uint32_t *target);

#endif /* DISASSEMBLE_M68K_H */
