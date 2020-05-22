// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#include <ctype.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>

#include "disassemble/m68k.h"

#include "psgplay/assert.h"
#include "psgplay/print.h"
#include "psgplay/sndh.h"

#include "system/unix/disassemble.h"
#include "system/unix/memory.h"
#include "system/unix/string.h"

struct memory {
	enum memory_type {
		MEMORY_UNDEFINED,
		MEMORY_TEXT,
		MEMORY_DATA,
	} type;
	const char *label;
	char *mnemonic;
	bool target;
};

struct disassembly {
	struct options *options;
	size_t size;
	const uint8_t *data;
	size_t header_size;
	struct memory *m;
};

static void dasm_label(struct disassembly *dasm, const char *s, size_t address)
{
	if (dasm->size <= address || dasm->m[address].label)
		return;

	dasm->m[address].label = s;
}

static int dasm_print_nl(struct disassembly *dasm, int i, int col)
{
	if (!col)
		return 0;

	if (12 <= i - col && i - col < dasm->header_size) {
		for (int c = col; c < 8; c++)
			printf("     ");

		printf(" /* ");

		for (size_t k = 0; k < col; k++) {
			const char c = dasm->data[i - col + k];

			printf("%c", isprint(c) ? c : '.');
		}

		printf(" */");
	}

	printf("\n");

	return 0;
}

static void dasm_print_label(struct disassembly *dasm, size_t i)
{
	if (dasm->size <= i)
		return;

	if (dasm->m[i].label)
		printf("%s:\n", dasm->m[i].label);
	else
		printf("_%zx:\n", i);
}

static bool dasm_is_label(struct disassembly *dasm, size_t i)
{
	if (dasm->size <= i)
		return false;

	return dasm->m[i].label != NULL || dasm->m[i].target;
}

static void dasm_print_data(struct disassembly *dasm, size_t i, size_t size)
{
	int col;

	for (col = 0; i < size; i++) {
		if (dasm_is_label(dasm, i)) {
			col = dasm_print_nl(dasm, i, col);
			dasm_print_label(dasm, i);
		}

		if (!col)
			printf("\t.dc.b\t0x%02x", dasm->data[i]);
		else
			printf(",0x%02x", dasm->data[i]);

		if (++col == 8)
			col = dasm_print_nl(dasm, i + 1, col);
	}

	if (col)
		dasm_print_nl(dasm, i, col);
}

struct insn_disassembly {
	size_t size;
	char s[256];
	struct disassembly *dasm;
};

static int dasm_print_insn_fmt(void *arg, const char *fmt, ...)
{
	struct insn_disassembly *insn = arg;

	va_list ap;

	va_start(ap, fmt);
	const int r = vsnprintf(&insn->s[insn->size],
		sizeof(insn->s) - insn->size, fmt, ap);
	va_end(ap);

	if (r < 0)
		insn->s[insn->size] = '\0';
	else
		insn->size += r;

	return r;
}

static struct m68k_symbol dasm_address_label(void *arg, uint32_t address)
{
	struct insn_disassembly *insn = arg;

	if (!dasm_is_label(insn->dasm, address))
		return (struct m68k_symbol) { };

	struct m68k_symbol sym;

	if (insn->dasm->m[address].label) {
		strncpy(sym.s, insn->dasm->m[address].label, sizeof(sym.s) - 1);
		sym.s[sizeof(sym.s) - 1] = '\0';
	} else
		snprintf(sym.s, sizeof(sym.s), "_%" PRIx32, address);

	return sym;
}

static void dasm_print_insn(struct disassembly *dasm, size_t i, size_t size)
{
	while (i < size) {
		struct insn_disassembly insn;

		insn.size = 0;
		insn.s[0] = '\0';
		insn.dasm = dasm;

		const int s = m68k_disassemble_instruction(
			&dasm->data[i], size - i, i,
			dasm_address_label, dasm_print_insn_fmt, &insn);

		if (s <= 0 || size < i + s)
			return dasm_print_data(dasm, i, size);

		if (dasm_is_label(dasm, i))
			dasm_print_label(dasm, i);

		printf("\t%s\n", insn.s);

		i += s;
	}
}

static size_t dasm_section_size(struct disassembly *dasm, size_t i)
{
	size_t s;

	for (s = 0; i + s < dasm->size; s++)
		if (!s)
			continue;
		else if (dasm_is_label(dasm, i + s))
			break;
		else if (dasm->m[i].type != dasm->m[i + s].type)
			break;

	return s;
}

static void dasm_print(struct disassembly *dasm)
{
	size_t i = 0;

	while (i < dasm->size) {
		const size_t s = dasm_section_size(dasm, i);

		if (!s)
			break;

		(dasm->m[i].type == MEMORY_TEXT ?
			dasm_print_insn :
			dasm_print_data)(dasm, i, i + s);

		i += s;
	}
}

static void dasm_mark(struct disassembly *dasm,
	size_t i, size_t size, enum memory_type type)
{
	for (size_t k = 0; i + k < dasm->size && k < size; k++)
		dasm->m[i + k].type = type;
}

static void dasm_mark_text(struct disassembly *dasm, size_t i, size_t size)
{
	dasm_mark(dasm, i, size, MEMORY_TEXT);
}

static void dasm_mark_target(struct disassembly *dasm, size_t target)
{
	if (target < dasm->size)
		dasm->m[target].target = true;
}

static void dasm_mark_text_trace(struct disassembly *dasm, size_t i)
{
	while (i < dasm->size) {
		if (dasm->options->disassemble == DISASSEMBLE_TYPE_HEADER &&
		    i > 12)
			return;

		if (dasm->m[i].type != MEMORY_UNDEFINED)
			return;

		const size_t s = dasm_section_size(dasm, i);

		if (!s)
			return;

		uint32_t target;
		enum m68k_insn_type type;
		const size_t insn_size = m68k_disassemble_type_target(
			&dasm->data[i], s, i, &type, &target);

		if (!insn_size)
			return;

		dasm_mark_text(dasm, i, insn_size);
		i += insn_size;

		switch (type) {
		case m68k_insn_noninsn:
			return;
		case m68k_insn_nonbranch:
			continue;
		case m68k_insn_branch:
			dasm_mark_target(dasm, target);
			i = target;
			continue;
		case m68k_insn_condbranch:
		case m68k_insn_jsr:
			dasm_mark_target(dasm, target);
			dasm_mark_text_trace(dasm, target);
			continue;
		case m68k_insn_return:
			return;
		default:
			BUG();
		}
	}
}

void sndh_disassemble(struct options *options, struct file file)
{
	struct disassembly dasm = {
		.options = options,
		.size = file.size,
		.data = file.data,
		.header_size = sndh_header_size(file.data, file.size),
		.m = zalloc(sizeof(struct memory[file.size]))
	};

	dasm_label(&dasm, "init", 0);
	dasm_label(&dasm, "exit", 4);
	dasm_label(&dasm, "play", 8);
	dasm_label(&dasm, "sndh", 12);
	dasm_label(&dasm, "_init", sndh_init_address(file.data, file.size));
	dasm_label(&dasm, "_exit", sndh_exit_address(file.data, file.size));
	dasm_label(&dasm, "_play", sndh_play_address(file.data, file.size));
	dasm_label(&dasm, "_sndh", dasm.header_size);

	dasm_mark_text_trace(&dasm, 0);
	dasm_mark_text_trace(&dasm, 4);
	dasm_mark_text_trace(&dasm, 8);

	dasm_print(&dasm);

	free(dasm.m);
}
