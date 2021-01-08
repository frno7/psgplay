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
#include "internal/compare.h"
#include "internal/print.h"
#include "internal/string.h"

#include "m68k/m68k.h"
#include "m68k/m68kda.h"

#include "psgplay/psgplay.h"
#include "psgplay/sndh.h"

#include "atari/machine.h"
#include "atari/mmu.h"
#include "atari/trace.h"

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
	enum sndh_insn {
		SNDH_INSN_INIT = 1,
		SNDH_INSN_EXIT = 2,
		SNDH_INSN_PLAY = 4,
	} sndh_insn;
	const char *label;
	char *mnemonic;
	bool target;
};

struct disassembly {
	struct strbuf sb;
	struct options *options;
	size_t size;
	uint8_t *data;
	size_t header_size;
	struct memory *m;
	enum sndh_insn sndh_insn_run;
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
			sbprintf(&dasm->sb, "    ");

		sbprintf(&dasm->sb, "\t; ");

		for (size_t k = 0; k < col; k++) {
			const char c = dasm->data[i - col + k];

			sbprintf(&dasm->sb, "%c", isprint(c) ? c : '.');
		}
	}

	sbprintf(&dasm->sb, "\n");

	return 0;
}

static void dasm_print_label(struct disassembly *dasm, size_t i)
{
	if (dasm->size <= i)
		return;

	if (dasm->m[i].label)
		sbprintf(&dasm->sb, "%s:\n", dasm->m[i].label);
	else
		sbprintf(&dasm->sb, "_%zx:\n", i);
}

static void print_address(struct strbuf *sb,
	uint32_t address, const void *data, size_t size)
{
	const uint8_t *b = data;
	size_t length = 0;
	char code[32];

	code[0] = '\0';
	for (size_t i = 0; i + 1 < size; i += 2)
		length += snprintf(&code[length], sizeof(code) - length,
			"%s%02x%02x", !i ? "" : " ", b[i], b[i + 1]);

	sbprintf(sb, "%6x: %-24s\t", address, code);
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
		} else if (!col) {
			if (dasm->options->disassemble_address)
				print_address(&dasm->sb, i, &dasm->data[i],
					min_t(size_t, size -i, 8));

			sbprintf(&dasm->sb, "\tdc.b\t$%02x", dasm->data[i]);
		} else
			sbprintf(&dasm->sb, ",$%02x", dasm->data[i]);

		if (++col == 8)
			col = dasm_print_nl(dasm, i + 1, col);
	}

	if (col)
		dasm_print_nl(dasm, i, col);
}

struct insn_disassembly {
	size_t size;
	char s[256];
	uint32_t address;
	struct disassembly *dasm;
};

struct insn_symbol {
	char s[16];
};

static struct insn_symbol dasm_address_label(
	const int16_t displacement, struct insn_disassembly *insn)
{
	const uint32_t address = insn->address + displacement;

	if (!dasm_is_label(insn->dasm, address))
		return (struct insn_symbol) { };

	struct insn_symbol sym;

	if (insn->dasm->m[address].label) {
		strncpy(sym.s, insn->dasm->m[address].label,
			sizeof(sym.s) - 1);
		sym.s[sizeof(sym.s) - 1] = '\0';
	} else
		snprintf(sym.s, sizeof(sym.s), "_%" PRIx32, address);

	return sym;
}

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

static void format_pcdi(int16_t displacement, struct m68kda *da)
{
	struct insn_disassembly *insn = da->arg;
	const struct insn_symbol sym = dasm_address_label(displacement, insn);

	if (sym.s[0])
		da->format(da->arg, "%s(pc)", sym.s);
	else
		da->format(da->arg, "%d(pc)", displacement);
}

static void format_pcix(struct m68kda_brief_ext brief, struct m68kda *da)
{
	struct insn_disassembly *insn = da->arg;
	const struct insn_symbol sym = dasm_address_label(brief.d, insn);

	if (sym.s[0])
		da->format(da->arg, "%s(pc,%s%s)",
			sym.s, brief_register_symbol(brief),
			brief_size_symbol(brief));
	else
		da->format(da->arg, "%d(pc,%s%s)",
			brief.d, brief_register_symbol(brief),
			brief_size_symbol(brief));
}

static void format_bra(int16_t displacement, struct m68kda *da)
{
	struct insn_disassembly *insn = da->arg;
	const struct insn_symbol sym = dasm_address_label(displacement, insn);

	if (sym.s[0])
		da->format(da->arg, "%s", sym.s);
	else
		da->format(da->arg, "%d(pc)", displacement);
}

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

static uint32_t dasm_sndh_insn_types(
	struct disassembly *dasm, const size_t address, const size_t size)
{
	uint32_t sndh_insn_types = 0;

	for (size_t i = 0; i < size; i++)
		sndh_insn_types |= dasm->m[address + i].sndh_insn;

	return sndh_insn_types;
}

static void dasm_print_sndh_insn_types(struct disassembly *dasm,
	const char *s, size_t i, size_t insn_size)
{
	const uint32_t sndh_insn_types =
		dasm_sndh_insn_types(dasm, i, insn_size);

	if (!sndh_insn_types)
		return;

	struct line_column lc = string_line_column(
		s, (struct line_column) { .column = 1 });

	while (lc.column < 41) {
		lc = char_line_column('\t', lc);
		sbprintf(&dasm->sb, "\t");
	}

	sbprintf(&dasm->sb, "\t;");

	if (sndh_insn_types & SNDH_INSN_INIT) sbprintf(&dasm->sb, " init");
	if (sndh_insn_types & SNDH_INSN_EXIT) sbprintf(&dasm->sb, " exit");
	if (sndh_insn_types & SNDH_INSN_PLAY) sbprintf(&dasm->sb, " play");
}

static void dasm_print_insn(struct disassembly *dasm, size_t i, size_t size)
{
	const struct m68kda_elements elements = {
		.pcdi = format_pcdi,
		.pcix = format_pcix,
		.bra = format_bra,
	};

	while (i < size) {
		struct insn_disassembly insn;

		insn.size = 1;
		insn.s[0] = '\t';
		insn.s[1] = '\0';
		insn.address = i + sizeof(union m68kda_insn),
		insn.dasm = dasm;

		const struct m68kda_spec *spec = m68kda_insn_disassemble(
			&dasm->data[i], size - i,
			&elements, dasm_print_insn_fmt, &insn);
		if (!spec)
			return dasm_print_data(dasm, i, size);

		const size_t insn_size = m68kda_insn_size(spec);

		if (dasm_is_label(dasm, i))
			dasm_print_label(dasm, i);

		if (dasm->options->disassemble_address)
			print_address(&dasm->sb, i, &dasm->data[i], insn_size);

		sbprintf(&dasm->sb, "%s", insn.s);

		dasm_print_sndh_insn_types(dasm, insn.s, i, insn_size);

		sbprintf(&dasm->sb, "\n");

		i += insn_size;
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
	size_t i, size_t size, enum memory_type type,
	enum sndh_insn sndh_insn_type)
{
	for (size_t k = 0; i + k < dasm->size && k < size; k++) {
		dasm->m[i + k].type = type;
		dasm->m[i + k].sndh_insn |= sndh_insn_type;
	}
}

static void dasm_mark_text(struct disassembly *dasm,
	size_t i, size_t size, enum sndh_insn sndh_insn_type)
{
	dasm_mark(dasm, i, size, MEMORY_TEXT, sndh_insn_type);
}

static void dasm_mark_target(struct disassembly *dasm, size_t target)
{
	if (target < dasm->size)
		dasm->m[target].target = true;
}

struct target {
	struct disassembly *dasm;
	uint32_t address;
	int32_t branch;
};

static void target_pcdi(int16_t displacement, struct m68kda *da)
{
	struct target *target = da->arg;

	dasm_mark_target(target->dasm, target->address + displacement);
}

static void target_pcix(struct m68kda_brief_ext brief, struct m68kda *da)
{
	struct target *target = da->arg;

	dasm_mark_target(target->dasm, target->address + brief.d);
}

static void target_bra(int16_t displacement, struct m68kda *da)
{
	struct target *target = da->arg;

	target->branch = target->address + displacement;

	dasm_mark_target(target->dasm, target->branch);
}

static void dasm_mark_text_trace(struct disassembly *dasm,
	size_t i, enum sndh_insn sndh_insn_type)
{
	const struct m68kda_elements elements = {
		.pcdi = target_pcdi,
		.pcix = target_pcix,
		.bra = target_bra,
	};

	while (i < dasm->size) {
		if (dasm->options->disassemble == DISASSEMBLE_TYPE_HEADER &&
		    i > 12)
			return;

		if (dasm->m[i].type != MEMORY_UNDEFINED &&
		    (~dasm->m[i].sndh_insn & sndh_insn_type) == 0)
			return;

		const size_t s = dasm_section_size(dasm, i);
		if (!s)
			return;

		struct target target = {
			.dasm = dasm,
			.address = i + sizeof(union m68kda_insn),
			.branch = -1
		};
		const struct m68kda_spec *spec = m68kda_insn_disassemble(
			&dasm->data[i], s, &elements, NULL, &target);
		if (!spec)
			return;

		const size_t insn_size = m68kda_insn_size(spec);
		const enum m68k_insn_type type = insn_type(spec->mnemonic);

		dasm_mark_text(dasm, i, insn_size, sndh_insn_type);
		i += insn_size;

		switch (type) {
		case m68k_insn_inv:
			return;
		case m68k_insn_ins:
			continue;
		case m68k_insn_jmp:
			if (target.branch == -1)
				return;
			i = target.branch;
			continue;
		case m68k_insn_jcc:
		case m68k_insn_jsr:
			if (target.branch != -1)
				dasm_mark_text_trace(dasm,
					target.branch, sndh_insn_type);
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
	dasm_mark_text_trace(dasm, 0, SNDH_INSN_INIT);
	dasm_mark_text_trace(dasm, 4, SNDH_INSN_EXIT);
	dasm_mark_text_trace(dasm, 8, SNDH_INSN_PLAY);
}

static void dasm_instruction(uint32_t pc, void *arg)
{
	struct disassembly *dasm = arg;

	if (pc < MACHINE_PROGRAM)
		return;

	const uint32_t i = pc - MACHINE_PROGRAM;

	dasm->sndh_insn_run =
		i == 0 ? SNDH_INSN_INIT :
		i == 4 ? SNDH_INSN_EXIT :
		i == 8 ? SNDH_INSN_PLAY : dasm->sndh_insn_run;

	dasm_mark_text_trace(dasm, i, dasm->sndh_insn_run);
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
	void (*insn_cb)(uint32_t pc, void *arg), void *arg,
	const struct options *options, struct file file)
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

		psgplay_instruction_callback(pp, insn_cb, arg);

		psgplay_read_stereo(pp, NULL, sample_count);

		psgplay_free(pp);
	}
}

void sndh_disassemble(struct options *options, struct file file)
{
	struct disassembly dasm = {
		.options = options,
		.size = file.size,
		.data = xmalloc(file.size),
		.header_size = sndh_header_size(file.data, file.size),
		.m = zalloc(sizeof(struct memory[file.size]))
	};

	memcpy(dasm.data, file.data, file.size);

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
		dasm_mark_text_trace_run(dasm_instruction,
			&dasm, options, file);

	dasm_print(&dasm);

	if (dasm.sb.length) {
		if (options->output) {
			if (!file_write(options->output,
					dasm.sb.s, dasm.sb.length))
				pr_fatal_errno(options->output);
		} else
			printf("%s", dasm.sb.s);
	}

	free(dasm.sb.s);
	free(dasm.data);
	free(dasm.m);
}

struct trace {
	struct options *options;
	struct strbuf sb;
};

static int trace_print_insn_fmt(void *arg, const char *fmt, ...)
{
	struct trace *trace = arg;

	va_list ap;

	va_start(ap, fmt);
	const int r = vsbprintf(&trace->sb, fmt, ap);
	va_end(ap);

	return r;
}

static void trace_reg(void)
{
	printf("reg %8" PRIu64, machine_cycle());

	printf(" pc %6" PRIx32, m68k_get_reg(NULL, M68K_REG_PC));

#define TRACE_REGS(reg)							\
	reg(sr, SR)							\
	reg(d0, D0) reg(d1, D1) reg(d2, D2) reg(d3, D3)			\
	reg(d4, D4) reg(d5, D5) reg(d6, D6) reg(d7, D7)			\
	reg(a0, A0) reg(a1, A1) reg(a2, A2) reg(a3, A3)			\
	reg(a4, A4) reg(a5, A5) reg(a6, A6)				\
	reg(usp, USP)							\
	reg(isp, ISP)
#define TRACE_REG(symbol_, label_)					\
	printf(" " #symbol_ " %x", m68k_get_reg(NULL, M68K_REG_ ## label_));
TRACE_REGS(TRACE_REG)

	printf("\n");
}

static void cpu_instruction_trace(uint32_t pc, void *arg)
{
	struct trace *trace = arg;

	if (!TRACE_ENABLE(&trace->options->trace, CPU))
		goto trace_reg;

	BUG_ON(pc % 2 != 0);

	trace->sb.length = 0;

	sbprintf(&trace->sb, "cpu %8" PRIu64 "  ", machine_cycle());

	const union m68kda_insn insn = { .word = probe_read_memory_16(pc) };
	const struct m68kda_spec *spec0 = m68kda_insn_find(insn);

	if (!spec0) {
		print_address(&trace->sb, pc, &insn, sizeof(insn));
		printf("%s\n", trace->sb.s);
		return;
	}

	const uint8_t insn_size = m68kda_insn_size(spec0);
	uint8_t buffer[32];

	BUG_ON(insn_size % 2 != 0);
	BUG_ON(insn_size < 2 || sizeof(buffer) < insn_size);

	probe_copy_memory_16(buffer, pc, insn_size / 2);

	print_address(&trace->sb, pc, buffer, insn_size);

	const struct m68kda_spec *spec1 = m68kda_insn_disassemble(
		buffer, insn_size, NULL, trace_print_insn_fmt, trace);

	BUG_ON(spec0 != spec1);
	BUG_ON(!trace->sb.length);

	printf("%s\n", trace->sb.s);

trace_reg:
	if (TRACE_ENABLE(&trace->options->trace, REG))
		trace_reg();
}

void sndh_trace(struct options *options, struct file file)
{
	struct trace trace = {
		.options = options,
	};

	dasm_mark_text_trace_run(cpu_instruction_trace, &trace, options, file);

	free(trace.sb.s);
}
