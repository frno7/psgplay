// SPDX-License-Identifier: GPL-2.0

#ifndef INTERNAL_CHECK_COMPILER_H
#define INTERNAL_CHECK_COMPILER_H

#if defined(__m68k__) && defined(__GNUC__) && defined(__linux__)
/*
 * Although m68k-linux-gcc with -march=68000 restricts itself to plain
 * 68000 instructions, it will occasionally emit unaligned 16- and 32-bit
 * memory access instructions which cause address exceptions on 68000
 * hardware. The reason is that Linux requires 68020+.
 *
 * m68k/GCC maintainers have indicated that they will not fix this bug,
 * and m68k-linux-gcc does not reject the -march=68000 option with an
 * error despite incompatibility with 68000 hardware which is why this
 * is tested here with the C preprocessor.
 *
 * The recommended compiler for 68000 hardware is m68k-elf-gcc.
 *
 * GCC bug 98627: https://gcc.gnu.org/bugzilla/show_bug.cgi?id=98627
 */
#error "m68k-linux-gcc always emits 68020, please use m68k-elf-gcc instead"
#error "See GCC bug 98627: https://gcc.gnu.org/bugzilla/show_bug.cgi?id=98627"
#endif

#endif /* INTERNAL_CHECK_COMPILER_H */
