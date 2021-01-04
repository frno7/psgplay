// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2021 Fredrik Noring
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "internal/assert.h"
#include "internal/print.h"
#include "internal/string.h"
#include "internal/types.h"

#include "m68k/m68kda.h"

#include "system/unix/file.h"
#include "system/unix/string.h"

/**
 * M68KDG_OPERAND_SIZE(op) - operand sizes
 * @c: category
 * @p: placement
 * @size: size in bytes, or -1 to determine by effective address (ea)
 */
#define M68KDG_OPERAND_SIZE(op)						\
	op('@', 'b', -1)						\
	op('@', 'd', -1)						\
	op('@', 'l', -1)						\
	op('@', 's', -1)						\
	op('@', 'w', -1)						\
	op('D', 'd',  0)						\
	op('D', 's',  0)						\
	op('A', 'd',  0)						\
	op('A', 's',  0)						\
	op('+', 'd',  0)						\
	op('+', 's',  0)						\
	op('-', 'd',  0)						\
	op('-', 's',  0)						\
	op('d', 's',  2)						\
	op('B', 'B',  0)						\
	op('B', 'w',  2)						\
	op('#', 'b',  2)						\
	op('#', 'w',  2)						\
	op('#', 'l',  4)						\
	op('M', 's',  0)						\
	op('Q', 'd',  0)						\
	op('S', 's',  0)						\
	op('S', 'd',  0)						\
	op('U', 'd',  0)						\
	op('C', 's',  0)						\
	op('C', 'd',  0)						\
	op('l', 'w',  2)						\
	op('L', 'w',  2)						\
	op('T', 's',  0)

struct fields {
	char *mnemonic;
	char *pattern;
	char *ea;
	char *op0;
	char *op1;
};

static struct m68kda_ea eap(const struct m68kda_opcp opcp,
	const union m68kda_insn opword)
{
	return (struct m68kda_ea) {
		.m = (opcp.p == 'd' ? opword.d_ea.m : opword.s_ea.m),
		.r = (opcp.p == 'd' ? opword.d_ea.r : opword.s_ea.r),
	};
}

static int8_t op_ea_size(const union m68kda_insn opword,
	const struct m68kda_opcp opcp)
{
	const struct m68kda_ea ea = eap(opcp, opword);

	return ea.m <= 4 ? 0 :
	       ea.m <= 6 ? 2 :
	       ea.r <= 0 ? 2 :
	       ea.r <= 1 ? 4 :
	       ea.r <= 3 ? 2 :
	       opcp.p == 'b' ? 2 :
	       opcp.p == 'w' ? 2 :
	       opcp.p == 'l' ? 4 : -1;
}

#define M68KDA_OPSPEC_INT(opcp)		(((opcp).c << 8) | (opcp).p)
#define M68KDA_CP_INT(c_, p_)		((c_ << 8) | p_)

static int8_t op_size(const union m68kda_insn opword,
	const struct m68kda_opcp opcp)
{
	switch (M68KDA_OPSPEC_INT(opcp))
	{
	case 0: return 0;
#define M68KDG_OPERAND_SIZE_CASE(c_, p_, size)				\
	case M68KDA_CP_INT(c_, p_):					\
		return size != -1 ? size : op_ea_size(opword, opcp);
M68KDG_OPERAND_SIZE(M68KDG_OPERAND_SIZE_CASE)
	default:
		BUG();
	}
}

static struct m68kda_opcp m68kda_opcp(const char *s)
{
	return (struct m68kda_opcp) {
		.c = s[0] != '.' ? s[0] : '\0',
		.p = s[1] != '.' ? s[1] : '\0',
	};
}

static uint16_t pattern_code(const char *p)
{
	const size_t n = strlen(p);
	uint16_t c = 0;

	for (size_t i = 0; i < n; i++) {
		BUG_ON(!strchr(".01", p[i]));

		c |= p[i] == '1' ? 1 << (n - 1 - i) : 0;
	}

	return c;
}

static uint16_t pattern_mask(const char *p)
{
	const size_t n = strlen(p);
	uint16_t c = 0;

	for (size_t i = 0; i < n; i++) {
		BUG_ON(!strchr(".01", p[i]));

		c |= p[i] != '.' ? 1 << (n - 1 - i) : 0;
	}

	return c;
}

static void insn_entry(struct strbuf *sb, const struct fields *f,
	const uint16_t code, const uint16_t mask)
{
	const uint16_t c = pattern_code(f->pattern) | code;
	const uint16_t m = pattern_mask(f->pattern) | mask;
	const union m68kda_insn opword = { .word = c };
	const int8_t op0_size = op_size(opword, m68kda_opcp(f->op0));
	const int8_t op1_size = op_size(opword, m68kda_opcp(f->op1));

	BUG_ON(op0_size == -1 || op1_size == -1);

	sbprintf(sb, "\top(\"%s\",\t0x%04x, 0x%04x", f->mnemonic, c, m);

	sbprintf(sb, ", %u", op0_size);
	if (f->op0[0] != '.')
		sbprintf(sb, ",  '%c',  '%c'", f->op0[0], f->op0[1]);
	else
		sbprintf(sb, ", '\\0', '\\0'");

	sbprintf(sb, ", %u", op1_size);
	if (f->op1[0] != '.')
		sbprintf(sb, ",  '%c',  '%c'", f->op1[0], f->op1[1]);
	else
		sbprintf(sb, ", '\\0', '\\0'");

	sbprintf(sb, ")\t\\\n");
}

static void insn_line(struct strbuf *sb, const struct fields *f)
{
	static const struct {
		uint16_t code;
		uint16_t mask;
	} m[] = {
		{ 0b000000, 0b111000 },	/* a: Dn         */
		{ 0b001000, 0b111000 },	/* d: An         */
		{ 0b010000, 0b111000 },	/* A: (An)       */
		{ 0b011000, 0b111000 },	/* +: (An)+      */
		{ 0b100000, 0b111000 },	/* -: -(An)      */
		{ 0b101000, 0b111000 },	/* D: (d16,An)   */
		{ 0b110000, 0b111000 },	/* X: (d8,An,Xk) */
		{ 0b111000, 0b111111 },	/* W: (xxx).w    */
		{ 0b111001, 0b111111 },	/* L: (xxx).l    */
		{ 0b111010, 0b111111 },	/* p: (d16,PC)   */
		{ 0b111011, 0b111111 },	/* x: (d8,PC,Xk) */
		{ 0b111100, 0b111111 },	/* I: #imm       */
	};

	BUG_ON(strlen(f->pattern) != 16);
	BUG_ON(strlen(f->ea) != 12);
	BUG_ON(strlen(f->op0) != 2);
	BUG_ON(strlen(f->op1) != 2);

	/*
	 * Expand all effective address combinations so that all table
	 * entries are valid instructions.
	 */

	if (strcmp(f->ea, "............") != 0) {
		for (size_t i = 0; f->ea[i]; i++)
			if (f->ea[i] != '.')
				insn_entry(sb, f, m[i].code, m[i].mask);
	} else
		insn_entry(sb, f, 0, 0);
}

static void insn_table(struct strbuf *sb, const char *s)
{
	struct string_split line;
	bool parse = false;

	sbprintf(sb, "#define M68KDG_INSTRUCTIONS(op)\t\t\t\t\t\t\\\n");

	for_each_string_split (line, s, "\n") {
		struct fields f = { };

		if (line.length > 0 && line.s[0] == '=') {
			parse = true;
		} else if (!parse || line.sep) {
			continue;
		} else if (sscanf(line.s, "%ms %ms %ms %ms %ms",
				&f.mnemonic, &f.pattern,
				&f.ea, &f.op0, &f.op1) == 5) {
			insn_line(sb, &f);
		}

		free(f.mnemonic);
		free(f.pattern);
		free(f.ea);
		free(f.op0);
		free(f.op1);
	}

	sbprintf(sb, "\n");
}

int main(int argc, char *argv[])
{
	if (argc != 4 || strcmp(argv[1], "-o") != 0)
		return EXIT_FAILURE;

	const char *out_filepath = argv[2];
	const char *in_filepath = argv[3];

	struct file spec = file_read(in_filepath);
	if (!file_valid(spec))
		pr_fatal_errno(in_filepath);

	struct strbuf sb = { };

	insn_table(&sb, spec.data);

	file_free(spec);

	return sb.length && file_write(out_filepath,
		O_WRONLY | O_CREAT | O_TRUNC, sb.s, sb.length) ?
		EXIT_SUCCESS : EXIT_FAILURE;
}
