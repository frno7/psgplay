// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#include <ctype.h>
#include <inttypes.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "internal/assert.h"
#include "internal/print.h"

#include "m68k/m68kda.h"

#include "psgplay/psgplay.h"
#include "psgplay/sndh.h"

#include "atari/machine.h"

#include "system/unix/disassemble.h"
#include "system/unix/memory.h"
#include "system/unix/remake.h"
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

static enum m68k_insn_type insn_type(const char *mnemonic)
{
	BUG_ON(!mnemonic);

	static const struct {
		const char *mnemonic;
		enum m68k_insn_type type;
	} types[] = {
		{ "bhi.w",	m68k_insn_jcc },
		{ "bls.w",	m68k_insn_jcc },
		{ "bcc.w",	m68k_insn_jcc },
		{ "bcs.w",	m68k_insn_jcc },
		{ "bne.w",	m68k_insn_jcc },
		{ "beq.w",	m68k_insn_jcc },
		{ "bvc.w",	m68k_insn_jcc },
		{ "bvs.w",	m68k_insn_jcc },
		{ "bpl.w",	m68k_insn_jcc },
		{ "bmi.w",	m68k_insn_jcc },
		{ "bge.w",	m68k_insn_jcc },
		{ "blt.w",	m68k_insn_jcc },
		{ "bgt.w",	m68k_insn_jcc },
		{ "ble.w",	m68k_insn_jcc },
		{ "bhi.s",	m68k_insn_jcc },
		{ "bls.s",	m68k_insn_jcc },
		{ "bcc.s",	m68k_insn_jcc },
		{ "bcs.s",	m68k_insn_jcc },
		{ "bne.s",	m68k_insn_jcc },
		{ "beq.s",	m68k_insn_jcc },
		{ "bvc.s",	m68k_insn_jcc },
		{ "bvs.s",	m68k_insn_jcc },
		{ "bpl.s",	m68k_insn_jcc },
		{ "bmi.s",	m68k_insn_jcc },
		{ "bge.s",	m68k_insn_jcc },
		{ "blt.s",	m68k_insn_jcc },
		{ "bgt.s",	m68k_insn_jcc },
		{ "ble.s",	m68k_insn_jcc },
		{ "bra.w",	m68k_insn_jmp },
		{ "bra.s",	m68k_insn_jmp },
		{ "bsr.w",	m68k_insn_jsr },
		{ "bsr.s",	m68k_insn_jsr },
		{ "dbcc",	m68k_insn_jcc },
		{ "dbcs",	m68k_insn_jcc },
		{ "dbeq",	m68k_insn_jcc },
		{ "dbf",	m68k_insn_jcc },
		{ "dbge",	m68k_insn_jcc },
		{ "dbgt",	m68k_insn_jcc },
		{ "dbhi",	m68k_insn_jcc },
		{ "dble",	m68k_insn_jcc },
		{ "dbls",	m68k_insn_jcc },
		{ "dblt",	m68k_insn_jcc },
		{ "dbmi",	m68k_insn_jcc },
		{ "dbne",	m68k_insn_jcc },
		{ "dbpl",	m68k_insn_jcc },
		{ "dbt",	m68k_insn_jcc },
		{ "dbvc",	m68k_insn_jcc },
		{ "dbvs",	m68k_insn_jcc },
		{ "jmp",	m68k_insn_jmp },
		{ "jsr",	m68k_insn_jsr },
		{ "rte",	m68k_insn_ret },
		{ "rtr",	m68k_insn_ret },
		{ "rts",	m68k_insn_ret },
	};

	for (size_t i = 0; i < ARRAY_SIZE(types); i++)
		if (strcmp(types[i].mnemonic, mnemonic) == 0)
			return types[i].type;

	return m68k_insn_ins;
}

static void dasm_label(struct disassembly *dasm, const char *s, size_t address)
{
	if (dasm->size <= address || dasm->m[address].label)
		return;

	dasm->m[address].label = s;
}

static bool dasm_is_header_remake(struct disassembly *dasm, size_t i)
{
	return dasm->options->remake_header && 12 <= i && i < dasm->header_size;
}

static bool dasm_is_label(struct disassembly *dasm, size_t i)
{
	if (dasm->size <= i)
		return false;

	return dasm->m[i].label != NULL || dasm->m[i].target;
}

static int dasm_print_nl(struct disassembly *dasm, int i, int col)
{
	if (!col || dasm_is_header_remake(dasm, i - col))
		return 0;

	if (12 <= i - col && i - col < dasm->header_size) {
		for (int c = col; c < 8; c++)
			printf("    ");

		printf(" ; ");

		for (size_t k = 0; k < col; k++) {
			const char c = dasm->data[i - col + k];

			printf("%c", isprint(c) ? c : '.');
		}
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

static void dasm_print_data(struct disassembly *dasm, size_t i, size_t size)
{
	int col;

	for (col = 0; i < size; i++) {
		if (dasm_is_label(dasm, i)) {
			col = dasm_print_nl(dasm, i, col);
			dasm_print_label(dasm, i);
		}

		if (dasm_is_header_remake(dasm, i)) {
			if (i == 12)
				remake_header(dasm->data, dasm->size);
		} else if (!col)
			printf("\tdc.b\t$%02x", dasm->data[i]);
		else
			printf(",$%02x", dasm->data[i]);

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

static struct m68kda_symbol dasm_address_label(void *arg, uint32_t address)
{
	struct insn_disassembly *insn = arg;

	if (!dasm_is_label(insn->dasm, address))
		return (struct m68kda_symbol) { };

	struct m68kda_symbol sym;

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

		const int s = m68kda_disassemble_instruction(
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
		const char *mnemonic;
		const size_t insn_size = m68kda_disassemble_type_target(
			&dasm->data[i], s, i, &mnemonic, &target);

		if (!insn_size)
			return;

		BUG_ON(!mnemonic);

		const enum m68k_insn_type type = insn_type(mnemonic);

		dasm_mark_text(dasm, i, insn_size);
		i += insn_size;

		switch (type) {
		case m68k_insn_inv:
			return;
		case m68k_insn_ins:
			continue;
		case m68k_insn_jmp:
			dasm_mark_target(dasm, target);
			i = target;
			continue;
		case m68k_insn_jcc:
		case m68k_insn_jsr:
			dasm_mark_target(dasm, target);
			dasm_mark_text_trace(dasm, target);
			continue;
		case m68k_insn_ret:
			return;
		default:
			BUG();
		}
	}
}

static void dasm_mark_text_trace_entries(struct disassembly *dasm)
{
	dasm_mark_text_trace(dasm, 0);	/* init */
	dasm_mark_text_trace(dasm, 4);	/* exit */
	dasm_mark_text_trace(dasm, 8);	/* play */
}

static void dasm_instruction(uint32_t pc, void *arg)
{
	struct disassembly *dasm = arg;

	if (pc < MACHINE_PROGRAM)
		return;

	dasm_mark_text_trace(dasm, pc - MACHINE_PROGRAM);
}

static ssize_t parse_time(const char *s, int frequency)
{
	float a, b;
	const int r = sscanf(s, "%f:%f", &a, &b);

	if (!r)
		pr_fatal_error("%s: malformed time '%s'\n", progname, s);

	return roundf((r == 2 ? 60.0f * a + b : a) * frequency);
}

static void dasm_mark_text_trace_run(
	struct disassembly *dasm,
	const struct options *options,
	struct file file)
{
	int subtune_count;

	const int frequency = 1000;
	const size_t sample_count = options->length ? parse_time(
		options->length, frequency) : 60 * frequency;

	if (!sample_count)
		return;

        if (!sndh_tag_subtune_count(&subtune_count, file.data, file.size))
                subtune_count = 1;

	for (int t = 1; t <= subtune_count; t++) {
		struct psgplay *pp = psgplay_init(
			file.data, file.size, t, frequency);

		if (!pp)
			continue;

		psgplay_instruction_callback(pp, dasm_instruction, dasm);

		psgplay_read_stereo(pp, NULL, sample_count);

		psgplay_free(pp);
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

	dasm_mark_text_trace_entries(&dasm);

	if (options->disassemble != DISASSEMBLE_TYPE_HEADER)
		dasm_mark_text_trace_run(&dasm, options, file);

	dasm_print(&dasm);

	free(dasm.m);
}
