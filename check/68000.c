/*
 * Check that the compiler generates 68000 when given -march=68000. Some
 * compilers such as m68k-linux-* emit 68020 despite given -march=68000 and
 * this will crash a 68000 processor.
 *
 * The recommended compiler target for -march=68000 is m68k-elf.
 *
 * Here is one example with where 68020 has been generated. The instruction
 * "clrl %a1@(1)" will cause an address error exception on the 68000 due to
 * the unaligned address:
 *
 * 00000000 <f>:
 *    0:	2049           	moveal %a1,%a0
 *    2:	202f 0004      	movel %sp@(4),%d0
 *    6:	42a9 0001      	clrl %a1@(1)
 *    a:	1280           	moveb %d0,%a1@
 *    c:	137c 0064 0003 	moveb #100,%a1@(3)
 *   12:	4e75           	rts
 *
 * GCC bug report: https://gcc.gnu.org/bugzilla/show_bug.cgi?id=98627
 */

struct s { char a, b, c, d, e; };

struct s f(char a)
{
	return (struct s) { .a = a, .d = 'd' };
}
