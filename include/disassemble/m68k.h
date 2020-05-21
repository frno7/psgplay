// SPDX-License-Identifier: GPL-2.0

#ifndef DISASSEMBLE_M68K_H
#define DISASSEMBLE_M68K_H

#include <stddef.h>
#include <stdint.h>

enum m68k_insn_type {
	m68k_insn_noninsn,		/* Not a valid instruction */
	m68k_insn_nonbranch,		/* Not a branch instruction */
	m68k_insn_branch,		/* Unconditional branch */
	m68k_insn_condbranch,		/* Conditional branch */
	m68k_insn_jsr,			/* Jump to subroutine */
	m68k_insn_return,		/* Subroutine return */
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
