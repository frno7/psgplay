// SPDX-License-Identifier: GPL-2.0

#ifndef DISASSEMBLE_M68K_H
#define DISASSEMBLE_M68K_H

#include <stddef.h>
#include <stdint.h>

struct m68k_symbol {
	char s[16];
};

int m68k_disassemble_instruction(const void *data, size_t size,
	uint32_t address,
	struct m68k_symbol (*symbol)(void *arg, uint32_t address),
	int (*print)(void *arg, const char *fmt, ...),
	void *arg);

int m68k_disassemble_type_target(const void *data, size_t size,
	uint32_t address, const char **type, uint32_t *target);

#endif /* DISASSEMBLE_M68K_H */
