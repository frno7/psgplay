// SPDX-License-Identifier: GPL-2.0
/*
 * Opcode table header for m680[01234]0/m6888[12].
 * Copyright 1989, 1991, 1992, 1993, 1994, 1995, 1996, 1997, 1999, 2001,
 * 2003, 2004 Free Software Foundation, Inc.
 */

#include <errno.h>
#include <inttypes.h>
#include <setjmp.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "internal/assert.h"

#include "disassemble/m68k.h"

typedef uint64_t bfd_vma;
typedef int64_t bfd_signed_vma;
#define sprintf_vma(s,x) sprintf (s, "%0" PRIx64, x)
#define snprintf_vma(s,ss,x) snprintf (s, ss, "%0" PRIx64, x)

typedef int (*fprintf_function)(void *f, const char *fmt, ...)
	__attribute__((format(printf, 2, 3)));

struct insn_memory {
	size_t size;
	const uint8_t *data;
	uint32_t address;
};

#define bfd_mach_m68000 1

/* This struct is passed into the instruction decoding routine,
   and is passed back out into each callback.  The various fields are used
   for conveying information from your main routine into your callbacks,
   for passing information into the instruction decoders (such as the
   addresses of the callback functions), or for passing information
   back from the instruction decoders to their callers.

   It must be initialized before it is first passed; this can be done
   by hand, or using one of the initialization macros below.  */

typedef struct disassemble_info {
  const struct insn_memory *insn_memory;
  fprintf_function fprintf_func;
  void *stream;

  /* Target description.  We could replace this with a pointer to the bfd,
     but that would require one.  There currently isn't any such requirement
     so to avoid introducing one we record these explicitly.  */
  /* The bfd_arch value.  */
  unsigned int arch;
  /* The bfd_mach value.  */
  unsigned long mach;

  void *private_data;

  /* Function used to get bytes to disassemble.  MEMADDR is the
     address of the stuff to be disassembled, MYADDR is the address to
     put the bytes in, and LENGTH is the number of bytes to read.
     INFO is a pointer to this struct.
     Returns an errno value or 0 for success.  */
  int (*read_memory_func)
    (bfd_vma memaddr, uint8_t *myaddr, int length,
	     struct disassemble_info *info);

  /* Function which should be called if we get an error that we can't
     recover from.  STATUS is the errno value from read_memory_func and
     MEMADDR is the address that we were trying to read.  INFO is a
     pointer to this struct.  */
  void (*memory_error_func)
    (int status, bfd_vma memaddr, struct disassemble_info *info);

  /* Function called to print ADDR.  */
  void (*print_address_func)
    (bfd_vma addr, struct disassemble_info *info);

  /* Function called to print ADDR.  */
  struct m68k_symbol (*symbol)
    (void *arg, uint32_t address);

  /* These are for buffer_read_memory.  */
  uint8_t *buffer;
  bfd_vma buffer_vma;
  int buffer_length;

  /* This variable may be set by the instruction decoder.  It suggests
      the number of bytes objdump should display on a single line.  If
      the instruction decoder sets this, it should always set it to
      the same value in order to get reasonable looking output.  */
  int bytes_per_line;

  /* Results from instruction decoders.  Not all decoders yet support
     this information.  This info is set each time an instruction is
     decoded, and is only valid for the last such instruction.

     To determine whether this decoder supports this information, set
     insn_info_valid to 0, decode an instruction, then check it.  */

  char insn_info_valid;		/* Branch info has been set. */
  char branch_delay_insns;	/* How many sequential insn's will run before
				   a branch takes effect.  (0 = normal) */
  char data_size;		/* Size of data reference in insn, in bytes */
  enum m68k_insn_type insn_type;	/* Type of instruction */
  bfd_vma target;		/* Target address of branch or dref, if known;
				   zero if unknown.  */
  bfd_vma target2;		/* Second target address for dref2 */

} disassemble_info;

/* These are used as bit flags for the arch field in the m68k_opcode structure. */
#define	_m68k_undef  0
#define	m68000   0x001

/* Handy aliases.  */
#define	m68000up   (m68000)

/* The structure used to hold information for an opcode.  */

struct m68k_opcode
{
  /* The opcode name.  */
  const char *name;
  /* The pseudo-size of the instruction(in bytes).  Used to determine
     number of bytes necessary to disassemble the instruction.  */
  unsigned int size;
  /* The opcode itself.  */
  unsigned long opcode;
  /* The mask used by the disassembler.  */
  unsigned long match;
  /* The arguments.  */
  const char *args;
  /* The architectures which support this opcode.  */
  unsigned int arch;
  /* Instruction type.  */
  enum m68k_insn_type insn_type;
};

/* We store four bytes of opcode for all opcodes because that is the
   most any of them need.  The actual length of an instruction is
   always at least 2 bytes, and is as much longer as necessary to hold
   the operands it has.

   The match field is a mask saying which bits must match particular
   opcode in order for an instruction to be an instance of that
   opcode.

   The args field is a string containing two characters for each
   operand of the instruction.  The first specifies the kind of
   operand; the second, the place it is stored.  */

/* Kinds of operands:
   Characters used: AaBbCcDdEeFfGgHIiJkLlMmnOopQqRrSsTtU VvWwXxYyZz01234|*~%;@!&$?/<>#^+-

   D  data register only.  Stored as 3 bits.
   A  address register only.  Stored as 3 bits.
   a  address register indirect only.  Stored as 3 bits.
   R  either kind of register.  Stored as 4 bits.
   r  either kind of register indirect only.  Stored as 4 bits.
      At the moment, used only for cas2 instruction.
   F  floating point coprocessor register only.   Stored as 3 bits.
   O  an offset (or width): immediate data 0-31 or data register.
      Stored as 6 bits in special format for BF... insns.
   +  autoincrement only.  Stored as 3 bits (number of the address register).
   -  autodecrement only.  Stored as 3 bits (number of the address register).
   Q  quick immediate data.  Stored as 3 bits.
      This matches an immediate operand only when value is in range 1 .. 8.
   M  moveq immediate data.  Stored as 8 bits.
      This matches an immediate operand only when value is in range -128..127
   T  trap vector immediate data.  Stored as 4 bits.

   k  K-factor for fmove.p instruction.   Stored as a 7-bit constant or
      a three bit register offset, depending on the field type.

   #  immediate data.  Stored in special places (b, w or l)
      which say how many bits to store.
   ^  immediate data for floating point instructions.   Special places
      are offset by 2 bytes from '#'...
   B  pc-relative address, converted to an offset
      that is treated as immediate data.
   d  displacement and register.  Stores the register as 3 bits
      and stores the displacement in the entire second word.

   C  the CCR.  No need to store it; this is just for filtering validity.
   S  the SR.  No need to store, just as with CCR.
   U  the USP.  No need to store, just as with CCR.
   E  the MAC ACC.  No need to store, just as with CCR.
   e  the EMAC ACC[0123].
   G  the MAC/EMAC MACSR.  No need to store, just as with CCR.
   g  the EMAC ACCEXT{01,23}.
   H  the MASK.  No need to store, just as with CCR.
   i  the MAC/EMAC scale factor.

   I  Coprocessor ID.   Not printed if 1.   The Coprocessor ID is always
      extracted from the 'd' field of word one, which means that an extended
      coprocessor opcode can be skipped using the 'i' place, if needed.

   s  System Control register for the floating point coprocessor.

    L  Register list of the type d0-d7/a0-a7 etc.
       (New!  Improved!  Can also hold fp0-fp7, as well!)
       The assembler tries to see if the registers match the insn by
       looking at where the insn wants them stored.

    l  Register list like L, but with all the bits reversed.
       Used for going the other way. . .

    c  cache identifier which may be "nc" for no cache, "ic"
       for instruction cache, "dc" for data cache, or "bc"
       for both caches.  Used in cinv and cpush.  Always
       stored in position "d".

    u  Any register, with ``upper'' or ``lower'' specification.  Used
       in the mac instructions with size word.

 The remainder are all stored as 6 bits using an address mode and a
 register number; they differ in which addressing modes they match.

   *  all					(modes 0-6,7.0-4)
   ~  alterable memory				(modes 2-6,7.0,7.1)
						(not 0,1,7.2-4)
   %  alterable					(modes 0-6,7.0,7.1)
						(not 7.2-4)
   ;  data					(modes 0,2-6,7.0-4)
						(not 1)
   @  data, but not immediate			(modes 0,2-6,7.0-3)
						(not 1,7.4)
   !  control					(modes 2,5,6,7.0-3)
						(not 0,1,3,4,7.4)
   &  alterable control				(modes 2,5,6,7.0,7.1)
						(not 0,1,3,4,7.2-4)
   $  alterable data				(modes 0,2-6,7.0,7.1)
						(not 1,7.2-4)
   ?  alterable control, or data register	(modes 0,2,5,6,7.0,7.1)
						(not 1,3,4,7.2-4)
   /  control, or data register			(modes 0,2,5,6,7.0-3)
						(not 1,3,4,7.4)
   >  *save operands				(modes 2,4,5,6,7.0,7.1)
						(not 0,1,3,7.2-4)
   <  *restore operands				(modes 2,3,5,6,7.0-3)
						(not 0,1,4,7.4)

   coldfire move operands:
   m  						(modes 0-4)
   n						(modes 5,7.2)
   o						(modes 6,7.0,7.1,7.3,7.4)
   p						(modes 0-5)

   coldfire bset/bclr/btst/mulsl/mulul operands:
   q						(modes 0,2-5)
   v						(modes 0,2-5,7.0,7.1)
   b                                            (modes 0,2-5,7.2)
   w                                            (modes 2-5,7.2)
   y						(modes 2,5)
   z						(modes 2,5,7.2)
   x  mov3q immediate operand.
   4						(modes 2,3,4,5)
  */

/* For the 68851:  */
/* I didn't use much imagination in choosing the
   following codes, so many of them aren't very
   mnemonic. -rab

   0  32 bit pmmu register
	Possible values:
	000	TC	Translation Control Register (68030, 68851)

   1  16 bit pmmu register
	111	AC	Access Control (68851)

   2  8 bit pmmu register
	100	CAL	Current Access Level (68851)
	101	VAL	Validate Access Level (68851)
	110	SCC	Stack Change Control (68851)

   3  68030-only pmmu registers (32 bit)
	010	TT0	Transparent Translation reg 0
			(aka Access Control reg 0 -- AC0 -- on 68ec030)
	011	TT1	Transparent Translation reg 1
			(aka Access Control reg 1 -- AC1 -- on 68ec030)

   W  wide pmmu registers
	Possible values:
	001	DRP	Dma Root Pointer (68851)
	010	SRP	Supervisor Root Pointer (68030, 68851)
	011	CRP	Cpu Root Pointer (68030, 68851)

   f	function code register (68030, 68851)
	0	SFC
	1	DFC

   V	VAL register only (68851)

   X	BADx, BACx (16 bit)
	100	BAD	Breakpoint Acknowledge Data (68851)
	101	BAC	Breakpoint Acknowledge Control (68851)

   Y	PSR (68851) (MMUSR on 68030) (ACUSR on 68ec030)
   Z	PCSR (68851)

   |	memory 		(modes 2-6, 7.*)

   t  address test level (68030 only)
      Stored as 3 bits, range 0-7.
      Also used for breakpoint instruction now.

*/

/* Places to put an operand, for non-general operands:
   Characters used: BbCcDdFfGgHhIijkLlMmNnostWw123456789/

   s  source, low bits of first word.
   d  dest, shifted 9 in first word
   1  second word, shifted 12
   2  second word, shifted 6
   3  second word, shifted 0
   4  third word, shifted 12
   5  third word, shifted 6
   6  third word, shifted 0
   7  second word, shifted 7
   8  second word, shifted 10
   9  second word, shifted 5
   D  store in both place 1 and place 3; for divul and divsl.
   B  first word, low byte, for branch displacements
   W  second word (entire), for branch displacements
   L  second and third words (entire), for branch displacements
      (also overloaded for move16)
   b  second word, low byte
   w  second word (entire) [variable word/long branch offset for dbra]
   W  second word (entire) (must be signed 16 bit value)
   l  second and third word (entire)
   g  variable branch offset for bra and similar instructions.
      The place to store depends on the magnitude of offset.
   t  store in both place 7 and place 8; for floating point operations
   c  branch offset for cpBcc operations.
      The place to store is word two if bit six of word one is zero,
      and words two and three if bit six of word one is one.
   i  Increment by two, to skip over coprocessor extended operands.   Only
      works with the 'I' format.
   k  Dynamic K-factor field.   Bits 6-4 of word 2, used as a register number.
      Also used for dynamic fmovem instruction.
   C  floating point coprocessor constant - 7 bits.  Also used for static
      K-factors...
   m  For M[S]ACx; 4 bits split with MSB shifted 6 bits in first word
      and remaining 3 bits of register shifted 9 bits in first word.
      Indicate upper/lower in 1 bit shifted 7 bits in second word.
      Use with `R' or `u' format.
   n  `m' withouth upper/lower indication. (For M[S]ACx; 4 bits split
      with MSB shifted 6 bits in first word and remaining 3 bits of
      register shifted 9 bits in first word.  No upper/lower
      indication is done.)  Use with `R' or `u' format.
   o  For M[S]ACw; 4 bits shifted 12 in second word (like `1').
      Indicate upper/lower in 1 bit shifted 7 bits in second word.
      Use with `R' or `u' format.
   M  For M[S]ACw; 4 bits in low bits of first word.  Indicate
      upper/lower in 1 bit shifted 6 bits in second word.  Use with
      `R' or `u' format.
   N  For M[S]ACw; 4 bits in low bits of second word.  Indicate
      upper/lower in 1 bit shifted 6 bits in second word.  Use with
      `R' or `u' format.
   h  shift indicator (scale factor), 1 bit shifted 10 in second word

 Places to put operand, for general operands:
   d  destination, shifted 6 bits in first word
   b  source, at low bit of first word, and immediate uses one byte
   w  source, at low bit of first word, and immediate uses two bytes
   l  source, at low bit of first word, and immediate uses four bytes
   s  source, at low bit of first word.
      Used sometimes in contexts where immediate is not allowed anyway.
   f  single precision float, low bit of 1st word, immediate uses 4 bytes
   F  double precision float, low bit of 1st word, immediate uses 8 bytes
   x  extended precision float, low bit of 1st word, immediate uses 12 bytes
   p  packed float, low bit of 1st word, immediate uses 12 bytes
   G  EMAC accumulator, load  (bit 4 2nd word, !bit8 first word)
   H  EMAC accumulator, non load  (bit 4 2nd word, bit 8 first word)
   F  EMAC ACCx
   f  EMAC ACCy
   I  MAC/EMAC scale factor
   /  Like 's', but set 2nd word, bit 5 if trailing_ampersand set
   ]  first word, bit 10
*/

extern const struct m68k_opcode m68k_opcodes[];

extern const int m68k_numopcodes;

/* Local function prototypes.  */

static const char * const fpcr_names[] =
{
  "", "%fpiar", "%fpsr", "%fpiar/%fpsr", "%fpcr",
  "%fpiar/%fpcr", "%fpsr/%fpcr", "%fpiar/%fpsr/%fpcr"
};

static const char *const reg_names[] =
{
  "%d0", "%d1", "%d2", "%d3", "%d4", "%d5", "%d6", "%d7",
  "%a0", "%a1", "%a2", "%a3", "%a4", "%a5", "%a6", "%sp",
  "%ps", "%pc"
};

/* Name of register halves for MAC/EMAC.
   Separate from reg_names since 'spu', 'fpl' look weird.  */
static const char *const reg_half_names[] =
{
  "%d0", "%d1", "%d2", "%d3", "%d4", "%d5", "%d6", "%d7",
  "%a0", "%a1", "%a2", "%a3", "%a4", "%a5", "%a6", "%a7",
  "%ps", "%pc"
};

/* Sign-extend an (unsigned char).  */
#if __STDC__ == 1
#define COERCE_SIGNED_CHAR(ch) ((signed char) (ch))
#else
#define COERCE_SIGNED_CHAR(ch) ((int) (((ch) ^ 0x80) & 0xFF) - 128)
#endif

/* Get a 1 byte signed integer.  */
#define NEXTBYTE(p)  (p += 2, fetch_data(info, p), COERCE_SIGNED_CHAR(p[-1]))

/* Get a 2 byte signed integer.  */
#define COERCE16(x) ((int) (((x) ^ 0x8000) - 0x8000))
#define NEXTWORD(p)  \
  (p += 2, fetch_data(info, p), \
   COERCE16 ((p[-2] << 8) + p[-1]))

/* Get a 4 byte signed integer.  */
#define COERCE32(x) ((bfd_signed_vma) ((x) ^ 0x80000000) - 0x80000000)
#define NEXTLONG(p)  \
  (p += 4, fetch_data(info, p), \
   (COERCE32 ((((((p[-4] << 8) + p[-3]) << 8) + p[-2]) << 8) + p[-1])))

/* Get a 4 byte unsigned integer.  */
#define NEXTULONG(p)  \
  (p += 4, fetch_data(info, p), \
   (unsigned int) ((((((p[-4] << 8) + p[-3]) << 8) + p[-2]) << 8) + p[-1]))

/* Need a function to convert from packed to double
   precision.   Actually, it's easier to print a
   packed number than a double anyway, so maybe
   there should be a special case to handle this... */
#define NEXTPACKED(p) \
  (p += 12, fetch_data(info, p), 0.0)

/* Maximum length of an instruction.  */
#define MAXLEN 22

struct private
{
  /* Points to first byte not fetched.  */
  uint8_t *max_fetched;
  uint8_t the_buffer[MAXLEN];
  bfd_vma insn_start;
  sigjmp_buf bailout;
};

/* Make sure that bytes from INFO->PRIVATE_DATA->BUFFER (inclusive)
   to ADDR (exclusive) are valid.  Returns 1 for success, longjmps
   on error.  */
static int
fetch_data2(struct disassemble_info *info, uint8_t *addr)
{
  int status;
  struct private *priv = (struct private *)info->private_data;
  bfd_vma start = priv->insn_start + (priv->max_fetched - priv->the_buffer);

  status = (*info->read_memory_func) (start,
				      priv->max_fetched,
				      addr - priv->max_fetched,
				      info);
  if (status != 0)
    {
      (*info->memory_error_func) (status, start, info);
      siglongjmp(priv->bailout, 1);
    }
  else
    priv->max_fetched = addr;
  return 1;
}

static int
fetch_data(struct disassemble_info *info, uint8_t *addr)
{
    if (addr <= ((struct private *) (info->private_data))->max_fetched) {
        return 1;
    } else {
        return fetch_data2(info, addr);
    }
}

/* This function is used to print to the bit-bucket.  */
static int
ignore_print (void *stream, const char *format, ...)
{
  return 0;
}

static void
dummy_print_address (bfd_vma vma, struct disassemble_info *info)
{
}

/* Fetch BITS bits from a position in the instruction specified by CODE.
   CODE is a "place to put an argument", or 'x' for a destination
   that is a general address (mode and register).
   BUFFER contains the instruction.  */

static int
fetch_arg (unsigned char *buffer,
	   int code,
	   int bits,
	   disassemble_info *info)
{
  int val = 0;

  switch (code)
    {
    case '/': /* MAC/EMAC mask bit.  */
      val = buffer[3] >> 5;
      break;

    case 'G': /* EMAC ACC load.  */
      val = ((buffer[3] >> 3) & 0x2) | ((~buffer[1] >> 7) & 0x1);
      break;

    case 'H': /* EMAC ACC !load.  */
      val = ((buffer[3] >> 3) & 0x2) | ((buffer[1] >> 7) & 0x1);
      break;

    case ']': /* EMAC ACCEXT bit.  */
      val = buffer[0] >> 2;
      break;

    case 'I': /* MAC/EMAC scale factor.  */
      val = buffer[2] >> 1;
      break;

    case 'F': /* EMAC ACCx.  */
      val = buffer[0] >> 1;
      break;

    case 'f':
      val = buffer[1];
      break;

    case 's':
      val = buffer[1];
      break;

    case 'd':			/* Destination, for register or quick.  */
      val = (buffer[0] << 8) + buffer[1];
      val >>= 9;
      break;

    case 'x':			/* Destination, for general arg.  */
      val = (buffer[0] << 8) + buffer[1];
      val >>= 6;
      break;

    case 'k':
      fetch_data(info, buffer + 3);
      val = (buffer[3] >> 4);
      break;

    case 'C':
      fetch_data(info, buffer + 3);
      val = buffer[3];
      break;

    case '1':
      fetch_data(info, buffer + 3);
      val = (buffer[2] << 8) + buffer[3];
      val >>= 12;
      break;

    case '2':
      fetch_data(info, buffer + 3);
      val = (buffer[2] << 8) + buffer[3];
      val >>= 6;
      break;

    case '3':
      fetch_data(info, buffer + 3);
      val = (buffer[2] << 8) + buffer[3];
      break;

    case '4':
      fetch_data(info, buffer + 5);
      val = (buffer[4] << 8) + buffer[5];
      val >>= 12;
      break;

    case '5':
      fetch_data(info, buffer + 5);
      val = (buffer[4] << 8) + buffer[5];
      val >>= 6;
      break;

    case '6':
      fetch_data(info, buffer + 5);
      val = (buffer[4] << 8) + buffer[5];
      break;

    case '7':
      fetch_data(info, buffer + 3);
      val = (buffer[2] << 8) + buffer[3];
      val >>= 7;
      break;

    case '8':
      fetch_data(info, buffer + 3);
      val = (buffer[2] << 8) + buffer[3];
      val >>= 10;
      break;

    case '9':
      fetch_data(info, buffer + 3);
      val = (buffer[2] << 8) + buffer[3];
      val >>= 5;
      break;

    case 'e':
      val = (buffer[1] >> 6);
      break;

    case 'm':
      val = (buffer[1] & 0x40 ? 0x8 : 0)
	| ((buffer[0] >> 1) & 0x7)
	| (buffer[3] & 0x80 ? 0x10 : 0);
      break;

    case 'n':
      val = (buffer[1] & 0x40 ? 0x8 : 0) | ((buffer[0] >> 1) & 0x7);
      break;

    case 'o':
      val = (buffer[2] >> 4) | (buffer[3] & 0x80 ? 0x10 : 0);
      break;

    case 'M':
      val = (buffer[1] & 0xf) | (buffer[3] & 0x40 ? 0x10 : 0);
      break;

    case 'N':
      val = (buffer[3] & 0xf) | (buffer[3] & 0x40 ? 0x10 : 0);
      break;

    case 'h':
      val = buffer[2] >> 2;
      break;

    default:
      BUG();
    }

  switch (bits)
    {
    case 1:
      return val & 1;
    case 2:
      return val & 3;
    case 3:
      return val & 7;
    case 4:
      return val & 017;
    case 5:
      return val & 037;
    case 6:
      return val & 077;
    case 7:
      return val & 0177;
    case 8:
      return val & 0377;
    case 12:
      return val & 07777;
    default:
      BUG();
    }
}

/* Check if an EA is valid for a particular code.  This is required
   for the EMAC instructions since the type of source address determines
   if it is a EMAC-load instruciton if the EA is mode 2-5, otherwise it
   is a non-load EMAC instruction and the bits mean register Ry.
   A similar case exists for the movem instructions where the register
   mask is interpreted differently for different EAs.  */

static bool m68k_valid_ea(char code, int val)
{
	int mode, mask;

#define M(n0,n1,n2,n3,n4,n5,n6,n70,n71,n72,n73,n74)			\
	(n0 | n1 << 1 | n2 << 2 | n3 << 3 | n4 << 4 | n5 << 5 | n6 << 6	\
	 | n70 << 7 | n71 << 8 | n72 << 9 | n73 << 10 | n74 << 11)

	switch (code)
	{
		case '*': mask = M (1,1,1,1,1,1,1,1,1,1,1,1); break;
		case '~': mask = M (0,0,1,1,1,1,1,1,1,0,0,0); break;
		case '%': mask = M (1,1,1,1,1,1,1,1,1,0,0,0); break;
		case ';': mask = M (1,0,1,1,1,1,1,1,1,1,1,1); break;
		case '@': mask = M (1,0,1,1,1,1,1,1,1,1,1,0); break;
		case '!': mask = M (0,0,1,0,0,1,1,1,1,1,1,0); break;
		case '&': mask = M (0,0,1,0,0,1,1,1,1,0,0,0); break;
		case '$': mask = M (1,0,1,1,1,1,1,1,1,0,0,0); break;
		case '?': mask = M (1,0,1,0,0,1,1,1,1,0,0,0); break;
		case '/': mask = M (1,0,1,0,0,1,1,1,1,1,1,0); break;
		case '|': mask = M (0,0,1,0,0,1,1,1,1,1,1,0); break;
		case '>': mask = M (0,0,1,0,1,1,1,1,1,0,0,0); break;
		case '<': mask = M (0,0,1,1,0,1,1,1,1,1,1,0); break;
		case 'm': mask = M (1,1,1,1,1,0,0,0,0,0,0,0); break;
		case 'n': mask = M (0,0,0,0,0,1,0,0,0,1,0,0); break;
		case 'o': mask = M (0,0,0,0,0,0,1,1,1,0,1,1); break;
		case 'p': mask = M (1,1,1,1,1,1,0,0,0,0,0,0); break;
		case 'q': mask = M (1,0,1,1,1,1,0,0,0,0,0,0); break;
		case 'v': mask = M (1,0,1,1,1,1,0,1,1,0,0,0); break;
		case 'b': mask = M (1,0,1,1,1,1,0,0,0,1,0,0); break;
		case 'w': mask = M (0,0,1,1,1,1,0,0,0,1,0,0); break;
		case 'y': mask = M (0,0,1,0,0,1,0,0,0,0,0,0); break;
		case 'z': mask = M (0,0,1,0,0,1,0,0,0,1,0,0); break;
		case '4': mask = M (0,0,1,1,1,1,0,0,0,0,0,0); break;
		default:
			  BUG();
	}

#undef M

	mode = (val >> 3) & 7;
	if (mode == 7)
		mode += val & 7;

	return (mask & (1 << mode)) != 0;
}

/* Print a base register REGNO and displacement DISP, on INFO->STREAM.
   REGNO = -1 for pc, -2 for none (suppressed).  */

static void
print_base (int regno, bfd_vma disp, disassemble_info *info)
{
  if (regno == -1)
    {
      (*info->fprintf_func) (info->stream, "%%pc@(");
      (*info->print_address_func) (disp, info);
    }
  else
    {
      char buf[50];

      if (regno == -2)
	(*info->fprintf_func) (info->stream, "@(");
      else if (regno == -3)
	(*info->fprintf_func) (info->stream, "%%zpc@(");
      else
	(*info->fprintf_func) (info->stream, "%s@(", reg_names[regno]);

      sprintf_vma (buf, disp);
      (*info->fprintf_func) (info->stream, "%s", buf);
    }
}

/* Print an indexed argument.  The base register is BASEREG (-1 for pc).
   P points to extension word, in buffer.
   ADDR is the nominal core address of that extension word.  */

static unsigned char *
print_indexed (int basereg,
	       unsigned char *p,
	       bfd_vma addr,
	       disassemble_info *info)
{
  int word;
  static const char *const scales[] = { "", ":2", ":4", ":8" };
  bfd_vma base_disp;
  bfd_vma outer_disp;
  char buf[40];
  char vmabuf[50];

  word = NEXTWORD (p);

  /* Generate the text for the index register.
     Where this will be output is not yet determined.  */
  sprintf (buf, "%s:%c%s",
	   reg_names[(word >> 12) & 0xf],
	   (word & 0x800) ? 'l' : 'w',
	   scales[(word >> 9) & 3]);

  /* Handle the 68000 style of indexing.  */

  if ((word & 0x100) == 0)
    {
      base_disp = word & 0xff;
      if ((base_disp & 0x80) != 0)
	base_disp -= 0x100;
      if (basereg == -1)
	base_disp += addr;
      print_base (basereg, base_disp, info);
      (*info->fprintf_func) (info->stream, ",%s)", buf);
      return p;
    }

  /* Handle the generalized kind.  */
  /* First, compute the displacement to add to the base register.  */
  if (word & 0200)
    {
      if (basereg == -1)
	basereg = -3;
      else
	basereg = -2;
    }
  if (word & 0100)
    buf[0] = '\0';
  base_disp = 0;
  switch ((word >> 4) & 3)
    {
    case 2:
      base_disp = NEXTWORD (p);
      break;
    case 3:
      base_disp = NEXTLONG (p);
    }
  if (basereg == -1)
    base_disp += addr;

  /* Handle single-level case (not indirect).  */
  if ((word & 7) == 0)
    {
      print_base (basereg, base_disp, info);
      if (buf[0] != '\0')
	(*info->fprintf_func) (info->stream, ",%s", buf);
      (*info->fprintf_func) (info->stream, ")");
      return p;
    }

  /* Two level.  Compute displacement to add after indirection.  */
  outer_disp = 0;
  switch (word & 3)
    {
    case 2:
      outer_disp = NEXTWORD (p);
      break;
    case 3:
      outer_disp = NEXTLONG (p);
    }

  print_base (basereg, base_disp, info);
  if ((word & 4) == 0 && buf[0] != '\0')
    {
      (*info->fprintf_func) (info->stream, ",%s", buf);
      buf[0] = '\0';
    }
  sprintf_vma (vmabuf, outer_disp);
  (*info->fprintf_func) (info->stream, ")@(%s", vmabuf);
  if (buf[0] != '\0')
    (*info->fprintf_func) (info->stream, ",%s", buf);
  (*info->fprintf_func) (info->stream, ")");

  return p;
}

/* Returns number of bytes "eaten" by the operand, or
   return -1 if an invalid operand was found, or -2 if
   an opcode tabe error was found.
   ADDR is the pc for this arg to be relative to.  */

static int
print_insn_arg (const char *d,
		unsigned char *buffer,
		unsigned char *p0,
		bfd_vma addr,
		disassemble_info *info)
{
  int val = 0;
  int place = d[1];
  unsigned char *p = p0;
  int regno;
  const char *regname;
  unsigned char *p1;
  double flval;
  int flt_p;
  bfd_signed_vma disp;
  unsigned int uval;

  switch (*d)
    {
    case 'c':		/* Cache identifier.  */
      {
        static const char *const cacheFieldName[] = { "nc", "dc", "ic", "bc" };
        val = fetch_arg (buffer, place, 2, info);
        (*info->fprintf_func) (info->stream, "%s", cacheFieldName[val]);
        break;
      }

    case 'a':		/* Address register indirect only. Cf. case '+'.  */
      {
        (*info->fprintf_func)
	  (info->stream,
	   "%s@",
	   reg_names[fetch_arg (buffer, place, 3, info) + 8]);
        break;
      }

    case '_':		/* 32-bit absolute address for move16.  */
      {
        uval = NEXTULONG (p);
	(*info->print_address_func) (uval, info);
        break;
      }

    case 'C':
      (*info->fprintf_func) (info->stream, "%%ccr");
      break;

    case 'S':
      (*info->fprintf_func) (info->stream, "%%sr");
      break;

    case 'U':
      (*info->fprintf_func) (info->stream, "%%usp");
      break;

    case 'E':
      (*info->fprintf_func) (info->stream, "%%acc");
      break;

    case 'G':
      (*info->fprintf_func) (info->stream, "%%macsr");
      break;

    case 'H':
      (*info->fprintf_func) (info->stream, "%%mask");
      break;

    case 'Q':
      val = fetch_arg (buffer, place, 3, info);
      /* 0 means 8, except for the bkpt instruction... */
      if (val == 0 && d[1] != 's')
	val = 8;
      (*info->fprintf_func) (info->stream, "#%d", val);
      break;

    case 'x':
      val = fetch_arg (buffer, place, 3, info);
      /* 0 means -1.  */
      if (val == 0)
	val = -1;
      (*info->fprintf_func) (info->stream, "#%d", val);
      break;

    case 'M':
      if (place == 'h')
	{
	  static const char *const scalefactor_name[] = { "<<", ">>" };
	  val = fetch_arg (buffer, place, 1, info);
	  (*info->fprintf_func) (info->stream, "%s", scalefactor_name[val]);
	}
      else
	{
	  val = fetch_arg (buffer, place, 8, info);
	  if (val & 0x80)
	    val = val - 0x100;
	  (*info->fprintf_func) (info->stream, "#%d", val);
	}
      break;

    case 'T':
      val = fetch_arg (buffer, place, 4, info);
      (*info->fprintf_func) (info->stream, "#%d", val);
      break;

    case 'D':
      (*info->fprintf_func) (info->stream, "%s",
			     reg_names[fetch_arg (buffer, place, 3, info)]);
      break;

    case 'A':
      (*info->fprintf_func)
	(info->stream, "%s",
	 reg_names[fetch_arg (buffer, place, 3, info) + 010]);
      break;

    case 'R':
      (*info->fprintf_func)
	(info->stream, "%s",
	 reg_names[fetch_arg (buffer, place, 4, info)]);
      break;

    case 'r':
      regno = fetch_arg (buffer, place, 4, info);
      if (regno > 7)
	(*info->fprintf_func) (info->stream, "%s@", reg_names[regno]);
      else
	(*info->fprintf_func) (info->stream, "@(%s)", reg_names[regno]);
      break;

    case 'F':
      (*info->fprintf_func)
	(info->stream, "%%fp%d",
	 fetch_arg (buffer, place, 3, info));
      break;

    case 'O':
      val = fetch_arg (buffer, place, 6, info);
      if (val & 0x20)
	(*info->fprintf_func) (info->stream, "%s", reg_names[val & 7]);
      else
	(*info->fprintf_func) (info->stream, "%d", val);
      break;

    case '+':
      (*info->fprintf_func)
	(info->stream, "%s@+",
	 reg_names[fetch_arg (buffer, place, 3, info) + 8]);
      break;

    case '-':
      (*info->fprintf_func)
	(info->stream, "%s@-",
	 reg_names[fetch_arg (buffer, place, 3, info) + 8]);
      break;

    case 'k':
      if (place == 'k')
	(*info->fprintf_func)
	  (info->stream, "{%s}",
	   reg_names[fetch_arg (buffer, place, 3, info)]);
      else if (place == 'C')
	{
	  val = fetch_arg (buffer, place, 7, info);
	  if (val > 63)		/* This is a signed constant.  */
	    val -= 128;
	  (*info->fprintf_func) (info->stream, "{#%d}", val);
	}
      else
	return -2;
      break;

    case '#':
    case '^':
      p1 = buffer + (*d == '#' ? 2 : 4);
      if (place == 's')
	val = fetch_arg (buffer, place, 4, info);
      else if (place == 'C')
	val = fetch_arg (buffer, place, 7, info);
      else if (place == '8')
	val = fetch_arg (buffer, place, 3, info);
      else if (place == '3')
	val = fetch_arg (buffer, place, 8, info);
      else if (place == 'b')
	val = NEXTBYTE (p1);
      else if (place == 'w' || place == 'W')
	val = NEXTWORD (p1);
      else if (place == 'l')
	val = NEXTLONG (p1);
      else
	return -2;
      (*info->fprintf_func) (info->stream, "#%d", val);
      break;

    case 'B':
      if (place == 'b')
	disp = NEXTBYTE (p);
      else if (place == 'B')
	disp = COERCE_SIGNED_CHAR (buffer[1]);
      else if (place == 'w' || place == 'W')
	disp = NEXTWORD (p);
      else if (place == 'l' || place == 'L' || place == 'C')
	disp = NEXTLONG (p);
      else if (place == 'g')
	{
	  disp = NEXTBYTE (buffer);
	  if (disp == 0)
	    disp = NEXTWORD (p);
	  else if (disp == -1)
	    disp = NEXTLONG (p);
	}
      else if (place == 'c')
	{
	  if (buffer[1] & 0x40)		/* If bit six is one, long offset.  */
	    disp = NEXTLONG (p);
	  else
	    disp = NEXTWORD (p);
	}
      else
	return -2;

      (*info->print_address_func) (addr + disp, info);
      break;

    case 'd':
      val = NEXTWORD (p);
      (*info->fprintf_func)
	(info->stream, "%s@(%d)",
	 reg_names[fetch_arg (buffer, place, 3, info) + 8], val);
      break;

    case 's':
      (*info->fprintf_func) (info->stream, "%s",
			     fpcr_names[fetch_arg (buffer, place, 3, info)]);
      break;

    case 'e':
      val = fetch_arg(buffer, place, 2, info);
      (*info->fprintf_func) (info->stream, "%%acc%d", val);
      break;

    case 'g':
      val = fetch_arg(buffer, place, 1, info);
      (*info->fprintf_func) (info->stream, "%%accext%s", val==0 ? "01" : "23");
      break;

    case 'i':
      val = fetch_arg(buffer, place, 2, info);
      if (val == 1)
	(*info->fprintf_func) (info->stream, "<<");
      else if (val == 3)
	(*info->fprintf_func) (info->stream, ">>");
      else
	return -1;
      break;

    case 'I':
      /* Get coprocessor ID... */
      val = fetch_arg (buffer, 'd', 3, info);

      if (val != 1)				/* Unusual coprocessor ID?  */
	(*info->fprintf_func) (info->stream, "(cpid=%d) ", val);
      break;

    case '4':
    case '*':
    case '~':
    case '%':
    case ';':
    case '@':
    case '!':
    case '$':
    case '?':
    case '/':
    case '&':
    case '|':
    case '<':
    case '>':
    case 'm':
    case 'n':
    case 'o':
    case 'p':
    case 'q':
    case 'v':
    case 'b':
    case 'w':
    case 'y':
    case 'z':
      if (place == 'd')
	{
	  val = fetch_arg (buffer, 'x', 6, info);
	  val = ((val & 7) << 3) + ((val >> 3) & 7);
	}
      else
	val = fetch_arg (buffer, 's', 6, info);

      /* If the <ea> is invalid for *d, then reject this match.  */
      if (!m68k_valid_ea (*d, val))
	return -1;

      /* Get register number assuming address register.  */
      regno = (val & 7) + 8;
      regname = reg_names[regno];
      switch (val >> 3)
	{
	case 0:
	  (*info->fprintf_func) (info->stream, "%s", reg_names[val]);
	  break;

	case 1:
	  (*info->fprintf_func) (info->stream, "%s", regname);
	  break;

	case 2:
	  (*info->fprintf_func) (info->stream, "%s@", regname);
	  break;

	case 3:
	  (*info->fprintf_func) (info->stream, "%s@+", regname);
	  break;

	case 4:
	  (*info->fprintf_func) (info->stream, "%s@-", regname);
	  break;

	case 5:
	  val = NEXTWORD (p);
	  (*info->fprintf_func) (info->stream, "%s@(%d)", regname, val);
	  break;

	case 6:
	  p = print_indexed (regno, p, addr, info);
	  break;

	case 7:
	  switch (val & 7)
	    {
	    case 0:
	      val = NEXTWORD (p);
	      (*info->print_address_func) (val, info);
	      break;

	    case 1:
	      uval = NEXTULONG (p);
	      (*info->print_address_func) (uval, info);
	      break;

	    case 2:
	      val = NEXTWORD (p);
	      (*info->fprintf_func) (info->stream, "%%pc@(");
	      (*info->print_address_func) (addr + val, info);
	      (*info->fprintf_func) (info->stream, ")");
	      break;

	    case 3:
	      p = print_indexed (-1, p, addr, info);
	      break;

	    case 4:
	      flt_p = 1;	/* Assume it's a float... */
	      switch (place)
	      {
		case 'b':
		  val = NEXTBYTE (p);
		  flt_p = 0;
		  break;

		case 'w':
		  val = NEXTWORD (p);
		  flt_p = 0;
		  break;

		case 'l':
		  val = NEXTLONG (p);
		  flt_p = 0;
		  break;

		case 'p':
		  flval = NEXTPACKED (p);
		  break;

		default:
		  return -1;
	      }
	      if (flt_p)	/* Print a float? */
		(*info->fprintf_func) (info->stream, "#%g", flval);
	      else
		(*info->fprintf_func) (info->stream, "#%d", val);
	      break;

	    default:
	      return -1;
	    }
	}

      /* If place is '/', then this is the case of the mask bit for
	 mac/emac loads. Now that the arg has been printed, grab the
	 mask bit and if set, add a '&' to the arg.  */
      if (place == '/')
	{
	  val = fetch_arg (buffer, place, 1, info);
	  if (val)
	    info->fprintf_func (info->stream, "&");
	}
      break;

    case 'L':
    case 'l':
	if (place == 'w')
	  {
	    char doneany;
	    p1 = buffer + 2;
	    val = NEXTWORD (p1);
	    /* Move the pointer ahead if this point is farther ahead
	       than the last.  */
	    p = p1 > p ? p1 : p;
	    if (val == 0)
	      {
		(*info->fprintf_func) (info->stream, "#0");
		break;
	      }
	    if (*d == 'l')
	      {
		int newval = 0;

		for (regno = 0; regno < 16; ++regno)
		  if (val & (0x8000 >> regno))
		    newval |= 1 << regno;
		val = newval;
	      }
	    val &= 0xffff;
	    doneany = 0;
	    for (regno = 0; regno < 16; ++regno)
	      if (val & (1 << regno))
		{
		  int first_regno;

		  if (doneany)
		    (*info->fprintf_func) (info->stream, "/");
		  doneany = 1;
		  (*info->fprintf_func) (info->stream, "%s", reg_names[regno]);
		  first_regno = regno;
		  while (val & (1 << (regno + 1)))
		    ++regno;
		  if (regno > first_regno)
		    (*info->fprintf_func) (info->stream, "-%s",
					   reg_names[regno]);
		}
	  }
	else if (place == '3')
	  {
	    /* `fmovem' insn.  */
	    char doneany;
	    val = fetch_arg (buffer, place, 8, info);
	    if (val == 0)
	      {
		(*info->fprintf_func) (info->stream, "#0");
		break;
	      }
	    if (*d == 'l')
	      {
		int newval = 0;

		for (regno = 0; regno < 8; ++regno)
		  if (val & (0x80 >> regno))
		    newval |= 1 << regno;
		val = newval;
	      }
	    val &= 0xff;
	    doneany = 0;
	    for (regno = 0; regno < 8; ++regno)
	      if (val & (1 << regno))
		{
		  int first_regno;
		  if (doneany)
		    (*info->fprintf_func) (info->stream, "/");
		  doneany = 1;
		  (*info->fprintf_func) (info->stream, "%%fp%d", regno);
		  first_regno = regno;
		  while (val & (1 << (regno + 1)))
		    ++regno;
		  if (regno > first_regno)
		    (*info->fprintf_func) (info->stream, "-%%fp%d", regno);
		}
	  }
	else if (place == '8')
	  {
	    /* fmoveml for FP status registers.  */
	    (*info->fprintf_func) (info->stream, "%s",
				   fpcr_names[fetch_arg (buffer, place, 3,
							 info)]);
	  }
	else
	  return -2;
      break;

    case 'X':
      place = '8';
      /* fall through */
    case 'Y':
    case 'Z':
    case 'W':
    case '0':
    case '1':
    case '2':
    case '3':
      {
	int val = fetch_arg (buffer, place, 5, info);
        const char *name = 0;

	switch (val)
	  {
	  case 2: name = "%tt0"; break;
	  case 3: name = "%tt1"; break;
	  case 0x10: name = "%tc"; break;
	  case 0x11: name = "%drp"; break;
	  case 0x12: name = "%srp"; break;
	  case 0x13: name = "%crp"; break;
	  case 0x14: name = "%cal"; break;
	  case 0x15: name = "%val"; break;
	  case 0x16: name = "%scc"; break;
	  case 0x17: name = "%ac"; break;
	  case 0x18: name = "%psr"; break;
	  case 0x19: name = "%pcsr"; break;
	  case 0x1c:
	  case 0x1d:
	    {
	      int break_reg = ((buffer[3] >> 2) & 7);

	      (*info->fprintf_func)
		(info->stream, val == 0x1c ? "%%bad%d" : "%%bac%d",
		 break_reg);
	    }
	    break;
	  default:
	    (*info->fprintf_func) (info->stream, "<mmu register %d>", val);
	  }
	if (name)
	  (*info->fprintf_func) (info->stream, "%s", name);
      }
      break;

    case 'f':
      {
	int fc = fetch_arg (buffer, place, 5, info);

	if (fc == 1)
	  (*info->fprintf_func) (info->stream, "%%dfc");
	else if (fc == 0)
	  (*info->fprintf_func) (info->stream, "%%sfc");
	else
	  /* xgettext:c-format */
	  (*info->fprintf_func) (info->stream, "<function code %d>", fc);
      }
      break;

    case 'V':
      (*info->fprintf_func) (info->stream, "%%val");
      break;

    case 't':
      {
	int level = fetch_arg (buffer, place, 3, info);

	(*info->fprintf_func) (info->stream, "%d", level);
      }
      break;

    case 'u':
      {
	short is_upper = 0;
	int reg = fetch_arg (buffer, place, 5, info);

	if (reg & 0x10)
	  {
	    is_upper = 1;
	    reg &= 0xf;
	  }
	(*info->fprintf_func) (info->stream, "%s%s",
			       reg_half_names[reg],
			       is_upper ? "u" : "l");
      }
      break;

    default:
      return -2;
    }

  return p - p0;
}

/* Try to match the current instruction to best and if so, return the
   number of bytes consumed from the instruction stream, else zero.  */

static int
match_insn_m68k (bfd_vma memaddr,
		 disassemble_info * info,
		 const struct m68k_opcode * best,
		 struct private * priv)
{
  unsigned char *save_p;
  unsigned char *p;
  const char *d;

  uint8_t *buffer = priv->the_buffer;
  fprintf_function save_printer = info->fprintf_func;
  void (* save_print_address) (bfd_vma, struct disassemble_info *)
    = info->print_address_func;

  /* Point at first word of argument data,
     and at descriptor for first argument.  */
  p = buffer + 2;

  /* Figure out how long the fixed-size portion of the instruction is.
     The only place this is stored in the opcode table is
     in the arguments--look for arguments which specify fields in the 2nd
     or 3rd words of the instruction.  */
  for (d = best->args; *d; d += 2)
    {
      /* I don't think it is necessary to be checking d[0] here;
	 I suspect all this could be moved to the case statement below.  */
      if (d[0] == '#')
	{
	  if (d[1] == 'l' && p - buffer < 6)
	    p = buffer + 6;
	  else if (p - buffer < 4 && d[1] != 'C' && d[1] != '8')
	    p = buffer + 4;
	}

      if ((d[0] == 'L' || d[0] == 'l') && d[1] == 'w' && p - buffer < 4)
	p = buffer + 4;

      switch (d[1])
	{
	case '1':
	case '2':
	case '3':
	case '7':
	case '8':
	case '9':
	case 'i':
	  if (p - buffer < 4)
	    p = buffer + 4;
	  break;
	case '4':
	case '5':
	case '6':
	  if (p - buffer < 6)
	    p = buffer + 6;
	  break;
	default:
	  break;
	}
    }

  /* pflusha is an exceptions.  It takes no arguments but is two words
     long.  Recognize it by looking at the lower 16 bits of the mask.  */
  if (p - buffer < 4 && (best->match & 0xFFFF) != 0)
    p = buffer + 4;

  /* lpstop is another exception.  It takes a one word argument but is
     three words long.  */
  if (p - buffer < 6
      && (best->match & 0xffff) == 0xffff
      && best->args[0] == '#'
      && best->args[1] == 'w')
    {
      /* Copy the one word argument into the usual location for a one
	 word argument, to simplify printing it.  We can get away with
	 this because we know exactly what the second word is, and we
	 aren't going to print anything based on it.  */
      p = buffer + 6;
      fetch_data(info, p);
      buffer[2] = buffer[4];
      buffer[3] = buffer[5];
    }

  fetch_data(info, p);

  d = best->args;

  info->insn_type = m68k_insn_noninsn;

  save_p = p;
  info->print_address_func = dummy_print_address;
  info->fprintf_func = ignore_print;

  /* We scan the operands twice.  The first time we don't print anything,
     but look for errors.  */
  for (; *d; d += 2)
    {
      int eaten = print_insn_arg (d, buffer, p, memaddr + (p - buffer), info);

      if (eaten >= 0)
	p += eaten;
      else if (eaten == -1)
	{
	  info->fprintf_func = save_printer;
	  info->print_address_func = save_print_address;
	  return 0;
	}
      else
	{
	  info->fprintf_func (info->stream,
			      /* xgettext:c-format */
			      "<internal error in opcode table: %s %s>\n",
			      best->name,  best->args);
	  info->fprintf_func = save_printer;
	  info->print_address_func = save_print_address;
	  return 2;
	}
    }

  p = save_p;
  info->fprintf_func = save_printer;
  info->print_address_func = save_print_address;

  d = best->args;

  info->insn_type = best->insn_type ? best->insn_type : m68k_insn_nonbranch;

  info->fprintf_func (info->stream, "%s", best->name);

  if (*d)
    info->fprintf_func (info->stream, "\t");

  while (*d)
    {
      p += print_insn_arg (d, buffer, p, memaddr + (p - buffer), info);
      d += 2;

      if (*d && *(d - 2) != 'I' && *d != 'k')
	info->fprintf_func (info->stream, ",");
    }

  return p - buffer;
}

/* Print the m68k instruction at address MEMADDR in debugged memory,
   on INFO->STREAM.  Returns length of the instruction, in bytes.  */

static int
print_insn_m68k (bfd_vma memaddr, disassemble_info *info)
{
  int i;
  const char *d;
  unsigned int arch_mask;
  struct private priv;
  uint8_t *buffer = priv.the_buffer;
  int major_opcode;
  static int numopcodes[16];
  static const struct m68k_opcode **opcodes[16];
  int val;

  if (!opcodes[0])
    {
      /* Speed up the matching by sorting the opcode
	 table on the upper four bits of the opcode.  */
      const struct m68k_opcode **opc_pointer[16];

      /* First count how many opcodes are in each of the sixteen buckets.  */
      for (i = 0; i < m68k_numopcodes; i++)
	numopcodes[(m68k_opcodes[i].opcode >> 28) & 15]++;

      /* Then create a sorted table of pointers
	 that point into the unsorted table.  */
      opc_pointer[0] = malloc (sizeof (struct m68k_opcode *)
                               * m68k_numopcodes);
      opcodes[0] = opc_pointer[0];

      for (i = 1; i < 16; i++)
	{
	  opc_pointer[i] = opc_pointer[i - 1] + numopcodes[i - 1];
	  opcodes[i] = opc_pointer[i];
	}

      for (i = 0; i < m68k_numopcodes; i++)
	*opc_pointer[(m68k_opcodes[i].opcode >> 28) & 15]++ = &m68k_opcodes[i];
    }

  info->private_data = (void *)&priv;
  /* Tell objdump to use two bytes per chunk
     and six bytes per line for displaying raw data.  */
  info->bytes_per_line = 6;
  priv.max_fetched = priv.the_buffer;
  priv.insn_start = memaddr;

  if (sigsetjmp(priv.bailout, 0) != 0) {
      /* Error return.  */
      return -1;
  }

  switch (info->mach)
    {
    default:
    case 0:
      arch_mask = (unsigned int) -1;
      break;
    case bfd_mach_m68000:
      arch_mask = m68000;
      break;
    }

  fetch_data(info, buffer + 2);
  major_opcode = (buffer[0] >> 4) & 15;

  for (i = 0; i < numopcodes[major_opcode]; i++)
    {
      const struct m68k_opcode *opc = opcodes[major_opcode][i];
      unsigned long opcode = opc->opcode;
      unsigned long match = opc->match;

      if (((0xff & buffer[0] & (match >> 24)) == (0xff & (opcode >> 24)))
	  && ((0xff & buffer[1] & (match >> 16)) == (0xff & (opcode >> 16)))
	  /* Only fetch the next two bytes if we need to.  */
	  && (((0xffff & match) == 0)
	      ||
              (fetch_data(info, buffer + 4)
	       && ((0xff & buffer[2] & (match >> 8)) == (0xff & (opcode >> 8)))
	       && ((0xff & buffer[3] & match) == (0xff & opcode)))
	      )
	  && (opc->arch & arch_mask) != 0)
	{
	  /* Don't use for printout the variants of divul and divsl
	     that have the same register number in two places.
	     The more general variants will match instead.  */
	  for (d = opc->args; *d; d += 2)
	    if (d[1] == 'D')
	      break;

	  /* Don't use for printout the variants of most floating
	     point coprocessor instructions which use the same
	     register number in two places, as above.  */
	  if (*d == '\0')
	    for (d = opc->args; *d; d += 2)
	      if (d[1] == 't')
		break;

	  /* Don't match fmovel with more than one register;
	     wait for fmoveml.  */
	  if (*d == '\0')
	    {
	      for (d = opc->args; *d; d += 2)
		{
		  if (d[0] == 's' && d[1] == '8')
		    {
		      val = fetch_arg (buffer, d[1], 3, info);
		      if ((val & (val - 1)) != 0)
			break;
		    }
		}
	    }

          /* Don't match FPU insns with non-default coprocessor ID.  */
          if (*d == '\0')
            {
              for (d = opc->args; *d; d += 2)
                {
                  if (d[0] == 'I')
                    {
                      val = fetch_arg (buffer, 'd', 3, info);
                      if (val != 1)
                        break;
                    }
                }
            }

	  if (*d == '\0')
	    if ((val = match_insn_m68k (memaddr, info, opc, & priv)))
	      return val;
	}
    }

  /* Handle undefined instructions.  */
  info->fprintf_func (info->stream, "0%o", (buffer[0] << 8) + buffer[1]);
  return 2;
}

#define one(x) ((unsigned int) (x) << 16)
#define two(x, y) (((unsigned int) (x) << 16) + (y))

/* The assembler requires that all instances of the same mnemonic must
   be consecutive.  If they aren't, the assembler will bomb at
   runtime.  */

const struct m68k_opcode m68k_opcodes[] =
{
{"abcd", 2,	one(0140400),	one(0170770), "DsDd", m68000up },
{"abcd", 2,	one(0140410),	one(0170770), "-s-d", m68000up },

{"addaw", 2,	one(0150300),	one(0170700), "*wAd", m68000up },
{"addal", 2,	one(0150700),	one(0170700), "*lAd", m68000up },

{"addib", 4,	one(0003000),	one(0177700), "#b$s", m68000up },
{"addiw", 4,	one(0003100),	one(0177700), "#w$s", m68000up },
{"addil", 6,	one(0003200),	one(0177700), "#l$s", m68000up },

{"addqb", 2,	one(0050000),	one(0170700), "Qd$b", m68000up },
{"addqw", 2,	one(0050100),	one(0170700), "Qd%w", m68000up },
{"addql", 2,	one(0050200),	one(0170700), "Qd%l", m68000up },

/* The add opcode can generate the adda, addi, and addq instructions.  */
{"addb", 2,	one(0050000),	one(0170700), "Qd$b", m68000up },
{"addb", 4,	one(0003000),	one(0177700), "#b$s", m68000up },
{"addb", 2,	one(0150000),	one(0170700), ";bDd", m68000up },
{"addb", 2,	one(0150400),	one(0170700), "Dd~b", m68000up },
{"addw", 2,	one(0050100),	one(0170700), "Qd%w", m68000up },
{"addw", 2,	one(0150300),	one(0170700), "*wAd", m68000up },
{"addw", 4,	one(0003100),	one(0177700), "#w$s", m68000up },
{"addw", 2,	one(0150100),	one(0170700), "*wDd", m68000up },
{"addw", 2,	one(0150500),	one(0170700), "Dd~w", m68000up },
{"addl", 2,	one(0050200),	one(0170700), "Qd%l", m68000up },
{"addl", 6,	one(0003200),	one(0177700), "#l$s", m68000up },
{"addl", 2,	one(0150700),	one(0170700), "*lAd", m68000up },
{"addl", 2,	one(0150200),	one(0170700), "*lDd", m68000up },
{"addl", 2,	one(0150600),	one(0170700), "Dd~l", m68000up },

{"addxb", 2,	one(0150400),	one(0170770), "DsDd", m68000up },
{"addxb", 2,	one(0150410),	one(0170770), "-s-d", m68000up },
{"addxw", 2,	one(0150500),	one(0170770), "DsDd", m68000up },
{"addxw", 2,	one(0150510),	one(0170770), "-s-d", m68000up },
{"addxl", 2,	one(0150600),	one(0170770), "DsDd", m68000up },
{"addxl", 2,	one(0150610),	one(0170770), "-s-d", m68000up },

{"andib", 4,	one(0001000),	one(0177700), "#b$s", m68000up },
{"andib", 4,	one(0001074),	one(0177777), "#bCs", m68000up },
{"andiw", 4,	one(0001100),	one(0177700), "#w$s", m68000up },
{"andiw", 4,	one(0001174),	one(0177777), "#wSs", m68000up },
{"andil", 6,	one(0001200),	one(0177700), "#l$s", m68000up },
{"andi", 4,	one(0001100),	one(0177700), "#w$s", m68000up },
{"andi", 4,	one(0001074),	one(0177777), "#bCs", m68000up },
{"andi", 4,	one(0001174),	one(0177777), "#wSs", m68000up },

/* The and opcode can generate the andi instruction.  */
{"andb", 4,	one(0001000),	one(0177700), "#b$s", m68000up },
{"andb", 4,	one(0001074),	one(0177777), "#bCs", m68000up },
{"andb", 2,	one(0140000),	one(0170700), ";bDd", m68000up },
{"andb", 2,	one(0140400),	one(0170700), "Dd~b", m68000up },
{"andw", 4,	one(0001100),	one(0177700), "#w$s", m68000up },
{"andw", 4,	one(0001174),	one(0177777), "#wSs", m68000up },
{"andw", 2,	one(0140100),	one(0170700), ";wDd", m68000up },
{"andw", 2,	one(0140500),	one(0170700), "Dd~w", m68000up },
{"andl", 6,	one(0001200),	one(0177700), "#l$s", m68000up },
{"andl", 2,	one(0140200),	one(0170700), ";lDd", m68000up },
{"andl", 2,	one(0140600),	one(0170700), "Dd~l", m68000up },
{"and", 4,	one(0001100),	one(0177700), "#w$w", m68000up },
{"and", 4,	one(0001074),	one(0177777), "#bCs", m68000up },
{"and", 4,	one(0001174),	one(0177777), "#wSs", m68000up },
{"and", 2,	one(0140100),	one(0170700), ";wDd", m68000up },
{"and", 2,	one(0140500),	one(0170700), "Dd~w", m68000up },

{"aslb", 2,	one(0160400),	one(0170770), "QdDs", m68000up },
{"aslb", 2,	one(0160440),	one(0170770), "DdDs", m68000up },
{"aslw", 2,	one(0160500),	one(0170770), "QdDs", m68000up },
{"aslw", 2,	one(0160540),	one(0170770), "DdDs", m68000up },
{"aslw", 2,	one(0160700),	one(0177700), "~s",   m68000up },
{"asll", 2,	one(0160600),	one(0170770), "QdDs", m68000up },
{"asll", 2,	one(0160640),	one(0170770), "DdDs", m68000up },

{"asrb", 2,	one(0160000),	one(0170770), "QdDs", m68000up },
{"asrb", 2,	one(0160040),	one(0170770), "DdDs", m68000up },
{"asrw", 2,	one(0160100),	one(0170770), "QdDs", m68000up },
{"asrw", 2,	one(0160140),	one(0170770), "DdDs", m68000up },
{"asrw", 2,	one(0160300),	one(0177700), "~s",   m68000up },
{"asrl", 2,	one(0160200),	one(0170770), "QdDs", m68000up },
{"asrl", 2,	one(0160240),	one(0170770), "DdDs", m68000up },

{"bhiw", 2,	one(0061000),	one(0177777), "BW", m68000up, m68k_insn_condbranch },
{"blsw", 2,	one(0061400),	one(0177777), "BW", m68000up, m68k_insn_condbranch },
{"bccw", 2,	one(0062000),	one(0177777), "BW", m68000up, m68k_insn_condbranch },
{"bcsw", 2,	one(0062400),	one(0177777), "BW", m68000up, m68k_insn_condbranch },
{"bnew", 2,	one(0063000),	one(0177777), "BW", m68000up, m68k_insn_condbranch },
{"beqw", 2,	one(0063400),	one(0177777), "BW", m68000up, m68k_insn_condbranch },
{"bvcw", 2,	one(0064000),	one(0177777), "BW", m68000up, m68k_insn_condbranch },
{"bvsw", 2,	one(0064400),	one(0177777), "BW", m68000up, m68k_insn_condbranch },
{"bplw", 2,	one(0065000),	one(0177777), "BW", m68000up, m68k_insn_condbranch },
{"bmiw", 2,	one(0065400),	one(0177777), "BW", m68000up, m68k_insn_condbranch },
{"bgew", 2,	one(0066000),	one(0177777), "BW", m68000up, m68k_insn_condbranch },
{"bltw", 2,	one(0066400),	one(0177777), "BW", m68000up, m68k_insn_condbranch },
{"bgtw", 2,	one(0067000),	one(0177777), "BW", m68000up, m68k_insn_condbranch },
{"blew", 2,	one(0067400),	one(0177777), "BW", m68000up, m68k_insn_condbranch },

{"bhis", 2,	one(0061000),	one(0177400), "BB", m68000up, m68k_insn_condbranch },
{"blss", 2,	one(0061400),	one(0177400), "BB", m68000up, m68k_insn_condbranch },
{"bccs", 2,	one(0062000),	one(0177400), "BB", m68000up, m68k_insn_condbranch },
{"bcss", 2,	one(0062400),	one(0177400), "BB", m68000up, m68k_insn_condbranch },
{"bnes", 2,	one(0063000),	one(0177400), "BB", m68000up, m68k_insn_condbranch },
{"beqs", 2,	one(0063400),	one(0177400), "BB", m68000up, m68k_insn_condbranch },
{"bvcs", 2,	one(0064000),	one(0177400), "BB", m68000up, m68k_insn_condbranch },
{"bvss", 2,	one(0064400),	one(0177400), "BB", m68000up, m68k_insn_condbranch },
{"bpls", 2,	one(0065000),	one(0177400), "BB", m68000up, m68k_insn_condbranch },
{"bmis", 2,	one(0065400),	one(0177400), "BB", m68000up, m68k_insn_condbranch },
{"bges", 2,	one(0066000),	one(0177400), "BB", m68000up, m68k_insn_condbranch },
{"blts", 2,	one(0066400),	one(0177400), "BB", m68000up, m68k_insn_condbranch },
{"bgts", 2,	one(0067000),	one(0177400), "BB", m68000up, m68k_insn_condbranch },
{"bles", 2,	one(0067400),	one(0177400), "BB", m68000up, m68k_insn_condbranch },

{"jhi", 2,	one(0061000),	one(0177400), "Bg", m68000up, m68k_insn_condbranch },
{"jls", 2,	one(0061400),	one(0177400), "Bg", m68000up, m68k_insn_condbranch },
{"jcc", 2,	one(0062000),	one(0177400), "Bg", m68000up, m68k_insn_condbranch },
{"jcs", 2,	one(0062400),	one(0177400), "Bg", m68000up, m68k_insn_condbranch },
{"jne", 2,	one(0063000),	one(0177400), "Bg", m68000up, m68k_insn_condbranch },
{"jeq", 2,	one(0063400),	one(0177400), "Bg", m68000up, m68k_insn_condbranch },
{"jvc", 2,	one(0064000),	one(0177400), "Bg", m68000up, m68k_insn_condbranch },
{"jvs", 2,	one(0064400),	one(0177400), "Bg", m68000up, m68k_insn_condbranch },
{"jpl", 2,	one(0065000),	one(0177400), "Bg", m68000up, m68k_insn_condbranch },
{"jmi", 2,	one(0065400),	one(0177400), "Bg", m68000up, m68k_insn_condbranch },
{"jge", 2,	one(0066000),	one(0177400), "Bg", m68000up, m68k_insn_condbranch },
{"jlt", 2,	one(0066400),	one(0177400), "Bg", m68000up, m68k_insn_condbranch },
{"jgt", 2,	one(0067000),	one(0177400), "Bg", m68000up, m68k_insn_condbranch },
{"jle", 2,	one(0067400),	one(0177400), "Bg", m68000up, m68k_insn_condbranch },

{"bchg", 2,	one(0000500),	one(0170700), "Dd$s", m68000up },
{"bchg", 4,	one(0004100),	one(0177700), "#b$s", m68000up },

{"bclr", 2,	one(0000600),	one(0170700), "Dd$s", m68000up },
{"bclr", 4,	one(0004200),	one(0177700), "#b$s", m68000up },

{"braw", 2,	one(0060000),	one(0177777), "BW", m68000up, m68k_insn_branch },
{"bras", 2,	one(0060000),	one(0177400), "BB", m68000up, m68k_insn_branch },

{"bset", 2,	one(0000700),	one(0170700), "Dd$s", m68000up },
{"bset", 4,	one(0004300),	one(0177700), "#b$s", m68000up },

{"bsrw", 2,	one(0060400),	one(0177777), "BW", m68000up, m68k_insn_jsr },
{"bsrs", 2,	one(0060400),	one(0177400), "BB", m68000up, m68k_insn_jsr },

{"btst", 2,	one(0000400),	one(0170700), "Dd;b", m68000up },
{"btst", 4,	one(0004000),	one(0177700), "#b@s", m68000up },

{"chkl", 2,	one(0040400),		one(0170700), ";lDd", m68000up },
{"chkw", 2,	one(0040600),		one(0170700), ";wDd", m68000up },

{"clrb", 2,	one(0041000),	one(0177700), "$s", m68000up },
{"clrw", 2,	one(0041100),	one(0177700), "$s", m68000up },
{"clrl", 2,	one(0041200),	one(0177700), "$s", m68000up },

{"cmpaw", 2,	one(0130300),	one(0170700), "*wAd", m68000up },
{"cmpal", 2,	one(0130700),	one(0170700), "*lAd", m68000up },

{"cmpib", 4,	one(0006000),	one(0177700), "#b@s", m68000up },
{"cmpiw", 4,	one(0006100),	one(0177700), "#w@s", m68000up },
{"cmpil", 6,	one(0006200),	one(0177700), "#l@s", m68000up },

{"cmpmb", 2,	one(0130410),	one(0170770), "+s+d", m68000up },
{"cmpmw", 2,	one(0130510),	one(0170770), "+s+d", m68000up },
{"cmpml", 2,	one(0130610),	one(0170770), "+s+d", m68000up },

/* The cmp opcode can generate the cmpa, cmpm, and cmpi instructions.  */
{"cmpb", 4,	one(0006000),	one(0177700), "#b@s", m68000up },
{"cmpb", 2,	one(0130410),	one(0170770), "+s+d", m68000up },
{"cmpb", 2,	one(0130000),	one(0170700), ";bDd", m68000up },
{"cmpw", 2,	one(0130300),	one(0170700), "*wAd", m68000up },
{"cmpw", 4,	one(0006100),	one(0177700), "#w@s", m68000up },
{"cmpw", 2,	one(0130510),	one(0170770), "+s+d", m68000up },
{"cmpw", 2,	one(0130100),	one(0170700), "*wDd", m68000up },
{"cmpl", 2,	one(0130700),	one(0170700), "*lAd", m68000up },
{"cmpl", 6,	one(0006200),	one(0177700), "#l@s", m68000up },
{"cmpl", 2,	one(0130610),	one(0170770), "+s+d", m68000up },
{"cmpl", 2,	one(0130200),	one(0170700), "*lDd", m68000up },

{"dbcc", 2,	one(0052310),	one(0177770), "DsBw", m68000up, m68k_insn_condbranch },
{"dbcs", 2,	one(0052710),	one(0177770), "DsBw", m68000up, m68k_insn_condbranch },
{"dbeq", 2,	one(0053710),	one(0177770), "DsBw", m68000up, m68k_insn_condbranch },
{"dbf", 2,	one(0050710),	one(0177770), "DsBw", m68000up, m68k_insn_condbranch },
{"dbge", 2,	one(0056310),	one(0177770), "DsBw", m68000up, m68k_insn_condbranch },
{"dbgt", 2,	one(0057310),	one(0177770), "DsBw", m68000up, m68k_insn_condbranch },
{"dbhi", 2,	one(0051310),	one(0177770), "DsBw", m68000up, m68k_insn_condbranch },
{"dble", 2,	one(0057710),	one(0177770), "DsBw", m68000up, m68k_insn_condbranch },
{"dbls", 2,	one(0051710),	one(0177770), "DsBw", m68000up, m68k_insn_condbranch },
{"dblt", 2,	one(0056710),	one(0177770), "DsBw", m68000up, m68k_insn_condbranch },
{"dbmi", 2,	one(0055710),	one(0177770), "DsBw", m68000up, m68k_insn_condbranch },
{"dbne", 2,	one(0053310),	one(0177770), "DsBw", m68000up, m68k_insn_condbranch },
{"dbpl", 2,	one(0055310),	one(0177770), "DsBw", m68000up, m68k_insn_condbranch },
{"dbt", 2,	one(0050310),	one(0177770), "DsBw", m68000up, m68k_insn_condbranch },
{"dbvc", 2,	one(0054310),	one(0177770), "DsBw", m68000up, m68k_insn_condbranch },
{"dbvs", 2,	one(0054710),	one(0177770), "DsBw", m68000up, m68k_insn_condbranch },

{"divuw", 2,	one(0100300),		one(0170700), ";wDd", m68000up },

{"eorib", 4,	one(0005000),	one(0177700), "#b$s", m68000up },
{"eorib", 4,	one(0005074),	one(0177777), "#bCs", m68000up },
{"eoriw", 4,	one(0005100),	one(0177700), "#w$s", m68000up },
{"eoriw", 4,	one(0005174),	one(0177777), "#wSs", m68000up },
{"eoril", 6,	one(0005200),	one(0177700), "#l$s", m68000up },
{"eori", 4,	one(0005074),	one(0177777), "#bCs", m68000up },
{"eori", 4,	one(0005174),	one(0177777), "#wSs", m68000up },
{"eori", 4,	one(0005100),	one(0177700), "#w$s", m68000up },

/* The eor opcode can generate the eori instruction.  */
{"eorb", 4,	one(0005000),	one(0177700), "#b$s", m68000up },
{"eorb", 4,	one(0005074),	one(0177777), "#bCs", m68000up },
{"eorb", 2,	one(0130400),	one(0170700), "Dd$s", m68000up },
{"eorw", 4,	one(0005100),	one(0177700), "#w$s", m68000up },
{"eorw", 4,	one(0005174),	one(0177777), "#wSs", m68000up },
{"eorw", 2,	one(0130500),	one(0170700), "Dd$s", m68000up },
{"eorl", 6,	one(0005200),	one(0177700), "#l$s", m68000up },
{"eorl", 2,	one(0130600),	one(0170700), "Dd$s", m68000up },
{"eor", 4,	one(0005074),	one(0177777), "#bCs", m68000up },
{"eor", 4,	one(0005174),	one(0177777), "#wSs", m68000up },
{"eor", 4,	one(0005100),	one(0177700), "#w$s", m68000up },
{"eor", 2,	one(0130500),	one(0170700), "Dd$s", m68000up },

{"exg", 2,	one(0140500),	one(0170770), "DdDs", m68000up },
{"exg", 2,	one(0140510),	one(0170770), "AdAs", m68000up },
{"exg", 2,	one(0140610),	one(0170770), "DdAs", m68000up },
{"exg", 2,	one(0140610),	one(0170770), "AsDd", m68000up },

{"extw", 2,	one(0044200),	one(0177770), "Ds", m68000up },
{"extl", 2,	one(0044300),	one(0177770), "Ds", m68000up },

{"illegal", 2,	one(0045374),	one(0177777), "",   m68000up },

{"jmp", 2,	one(0047300),	one(0177700), "!s", m68000up, m68k_insn_branch },

{"jra", 2,	one(0060000),	one(0177400), "Bg", m68000up },
{"jra", 2,	one(0047300),	one(0177700), "!s", m68000up },

{"jsr", 2,	one(0047200),	one(0177700), "!s", m68000up, m68k_insn_jsr },

{"jbsr", 2,	one(0060400),	one(0177400), "Bg", m68000up },
{"jbsr", 2,	one(0047200),	one(0177700), "!s", m68000up },

{"lea", 2,	one(0040700),	one(0170700), "!sAd", m68000up },

{"linkw", 4,	one(0047120),	one(0177770), "As#w", m68000up },
{"link", 4,	one(0047120),	one(0177770), "As#W", m68000up },

{"lslb", 2,	one(0160410),	one(0170770), "QdDs", m68000up },
{"lslb", 2,	one(0160450),	one(0170770), "DdDs", m68000up },
{"lslw", 2,	one(0160510),	one(0170770), "QdDs", m68000up },
{"lslw", 2,	one(0160550),	one(0170770), "DdDs", m68000up },
{"lslw", 2,	one(0161700),	one(0177700), "~s",   m68000up },
{"lsll", 2,	one(0160610),	one(0170770), "QdDs", m68000up },
{"lsll", 2,	one(0160650),	one(0170770), "DdDs", m68000up },

{"lsrb", 2,	one(0160010),	one(0170770), "QdDs", m68000up },
{"lsrb", 2,	one(0160050),	one(0170770), "DdDs", m68000up },
{"lsrw", 2,	one(0160110),	one(0170770), "QdDs", m68000up },
{"lsrw", 2,	one(0160150),	one(0170770), "DdDs", m68000up },
{"lsrw", 2,	one(0161300),	one(0177700), "~s",   m68000up },
{"lsrl", 2,	one(0160210),	one(0170770), "QdDs", m68000up },
{"lsrl", 2,	one(0160250),	one(0170770), "DdDs", m68000up },

{"moveal", 2,	one(0020100),	one(0170700), "*lAd", m68000up },
{"moveaw", 2,	one(0030100),	one(0170700), "*wAd", m68000up },

{"movemw", 4,	one(0044200),	one(0177700), "Lw&s", m68000up },
{"movemw", 4,	one(0044240),	one(0177770), "lw-s", m68000up },
{"movemw", 4,	one(0044200),	one(0177700), "#w>s", m68000up },
{"movemw", 4,	one(0046200),	one(0177700), "<sLw", m68000up },
{"movemw", 4,	one(0046200),	one(0177700), "<s#w", m68000up },
{"moveml", 4,	one(0044300),	one(0177700), "Lw&s", m68000up },
{"moveml", 4,	one(0044340),	one(0177770), "lw-s", m68000up },
{"moveml", 4,	one(0044300),	one(0177700), "#w>s", m68000up },
{"moveml", 4,	one(0046300),	one(0177700), "<sLw", m68000up },
{"moveml", 4,	one(0046300),	one(0177700), "<s#w", m68000up },

{"movepw", 2,	one(0000410),	one(0170770), "dsDd", m68000up },
{"movepw", 2,	one(0000610),	one(0170770), "Ddds", m68000up },
{"movepl", 2,	one(0000510),	one(0170770), "dsDd", m68000up },
{"movepl", 2,	one(0000710),	one(0170770), "Ddds", m68000up },

{"moveq", 2,	one(0070000),	one(0170400), "MsDd", m68000up },
{"moveq", 2,	one(0070000),	one(0170400), "#BDd", m68000up },

/* The move opcode can generate the movea and moveq instructions.  */
{"moveb", 2,	one(0010000),	one(0170000), ";b$d", m68000up },

{"movew", 2,	one(0030000),	one(0170000), "*w%d", m68000up },
{"movew", 2,	one(0040300),	one(0177700), "Ss$s", m68000up },
{"movew", 2,	one(0042300),	one(0177700), ";wCd", m68000up },
{"movew", 2,	one(0043300),	one(0177700), ";wSd", m68000up },

{"movel", 2,	one(0070000),	one(0170400), "MsDd", m68000up },
{"movel", 2,	one(0020000),	one(0170000), "*l%d", m68000up },
{"movel", 2,	one(0047140),	one(0177770), "AsUd", m68000up },
{"movel", 2,	one(0047150),	one(0177770), "UdAs", m68000up },

{"move", 2,	one(0030000),	one(0170000), "*w%d", m68000up },
{"move", 2,	one(0040300),	one(0177700), "Ss$s", m68000up },
{"move", 2,	one(0042300),	one(0177700), ";wCd", m68000up },
{"move", 2,	one(0043300),	one(0177700), ";wSd", m68000up },

{"move", 2,	one(0047140),	one(0177770), "AsUd", m68000up },
{"move", 2,	one(0047150),	one(0177770), "UdAs", m68000up },

{"mulsw", 2,	one(0140700),		one(0170700), ";wDd", m68000up },

{"muluw", 2,	one(0140300),		one(0170700), ";wDd", m68000up },

{"nbcd", 2,	one(0044000),	one(0177700), "$s", m68000up },

{"negb", 2,	one(0042000),	one(0177700), "$s", m68000up },
{"negw", 2,	one(0042100),	one(0177700), "$s", m68000up },
{"negl", 2,	one(0042200),	one(0177700), "$s", m68000up },

{"negxb", 2,	one(0040000),	one(0177700), "$s", m68000up },
{"negxw", 2,	one(0040100),	one(0177700), "$s", m68000up },
{"negxl", 2,	one(0040200),	one(0177700), "$s", m68000up },

{"nop", 2,	one(0047161),	one(0177777), "", m68000up },

{"notb", 2,	one(0043000),	one(0177700), "$s", m68000up },
{"notw", 2,	one(0043100),	one(0177700), "$s", m68000up },
{"notl", 2,	one(0043200),	one(0177700), "$s", m68000up },

{"orib", 4,	one(0000000),	one(0177700), "#b$s", m68000up },
{"orib", 4,	one(0000074),	one(0177777), "#bCs", m68000up },
{"oriw", 4,	one(0000100),	one(0177700), "#w$s", m68000up },
{"oriw", 4,	one(0000174),	one(0177777), "#wSs", m68000up },
{"oril", 6,	one(0000200),	one(0177700), "#l$s", m68000up },
{"ori", 4,	one(0000074),	one(0177777), "#bCs", m68000up },
{"ori", 4,	one(0000100),	one(0177700), "#w$s", m68000up },
{"ori", 4,	one(0000174),	one(0177777), "#wSs", m68000up },

/* The or opcode can generate the ori instruction.  */
{"orb", 4,	one(0000000),	one(0177700), "#b$s", m68000up },
{"orb", 4,	one(0000074),	one(0177777), "#bCs", m68000up },
{"orb", 2,	one(0100000),	one(0170700), ";bDd", m68000up },
{"orb", 2,	one(0100400),	one(0170700), "Dd~s", m68000up },
{"orw", 4,	one(0000100),	one(0177700), "#w$s", m68000up },
{"orw", 4,	one(0000174),	one(0177777), "#wSs", m68000up },
{"orw", 2,	one(0100100),	one(0170700), ";wDd", m68000up },
{"orw", 2,	one(0100500),	one(0170700), "Dd~s", m68000up },
{"orl", 6,	one(0000200),	one(0177700), "#l$s", m68000up },
{"orl", 2,	one(0100200),	one(0170700), ";lDd", m68000up },
{"orl", 2,	one(0100600),	one(0170700), "Dd~s", m68000up },
{"or", 4,	one(0000074),	one(0177777), "#bCs", m68000up },
{"or", 4,	one(0000100),	one(0177700), "#w$s", m68000up },
{"or", 4,	one(0000174),	one(0177777), "#wSs", m68000up },
{"or", 2,	one(0100100),	one(0170700), ";wDd", m68000up },
{"or", 2,	one(0100500),	one(0170700), "Dd~s", m68000up },

{"pea", 2,	one(0044100),		one(0177700), "!s", m68000up },

{"reset", 2,	one(0047160),		one(0177777), "", m68000up },

{"rolb", 2,	one(0160430),		one(0170770), "QdDs", m68000up },
{"rolb", 2,	one(0160470),		one(0170770), "DdDs", m68000up },
{"rolw", 2,	one(0160530),		one(0170770), "QdDs", m68000up },
{"rolw", 2,	one(0160570),		one(0170770), "DdDs", m68000up },
{"rolw", 2,	one(0163700),		one(0177700), "~s",   m68000up },
{"roll", 2,	one(0160630),		one(0170770), "QdDs", m68000up },
{"roll", 2,	one(0160670),		one(0170770), "DdDs", m68000up },

{"rorb", 2,	one(0160030),		one(0170770), "QdDs", m68000up },
{"rorb", 2,	one(0160070),		one(0170770), "DdDs", m68000up },
{"rorw", 2,	one(0160130),		one(0170770), "QdDs", m68000up },
{"rorw", 2,	one(0160170),		one(0170770), "DdDs", m68000up },
{"rorw", 2,	one(0163300),		one(0177700), "~s",   m68000up },
{"rorl", 2,	one(0160230),		one(0170770), "QdDs", m68000up },
{"rorl", 2,	one(0160270),		one(0170770), "DdDs", m68000up },

{"roxlb", 2,	one(0160420),		one(0170770), "QdDs", m68000up },
{"roxlb", 2,	one(0160460),		one(0170770), "DdDs", m68000up },
{"roxlw", 2,	one(0160520),		one(0170770), "QdDs", m68000up },
{"roxlw", 2,	one(0160560),		one(0170770), "DdDs", m68000up },
{"roxlw", 2,	one(0162700),		one(0177700), "~s",   m68000up },
{"roxll", 2,	one(0160620),		one(0170770), "QdDs", m68000up },
{"roxll", 2,	one(0160660),		one(0170770), "DdDs", m68000up },

{"roxrb", 2,	one(0160020),		one(0170770), "QdDs", m68000up },
{"roxrb", 2,	one(0160060),		one(0170770), "DdDs", m68000up },
{"roxrw", 2,	one(0160120),		one(0170770), "QdDs", m68000up },
{"roxrw", 2,	one(0160160),		one(0170770), "DdDs", m68000up },
{"roxrw", 2,	one(0162300),		one(0177700), "~s",   m68000up },
{"roxrl", 2,	one(0160220),		one(0170770), "QdDs", m68000up },
{"roxrl", 2,	one(0160260),		one(0170770), "DdDs", m68000up },

{"rte", 2,	one(0047163),		one(0177777), "",   m68000up },

{"rtr", 2,	one(0047167),		one(0177777), "",   m68000up },

{"rts", 2,	one(0047165),		one(0177777), "",   m68000up, m68k_insn_return },

{"sbcd", 2,	one(0100400),		one(0170770), "DsDd", m68000up },
{"sbcd", 2,	one(0100410),		one(0170770), "-s-d", m68000up },

{"scc", 2,	one(0052300),	one(0177700), "$s", m68000up },
{"scs", 2,	one(0052700),	one(0177700), "$s", m68000up },
{"seq", 2,	one(0053700),	one(0177700), "$s", m68000up },
{"sf", 2,	one(0050700),	one(0177700), "$s", m68000up },
{"sge", 2,	one(0056300),	one(0177700), "$s", m68000up },
{"sgt", 2,	one(0057300),	one(0177700), "$s", m68000up },
{"shi", 2,	one(0051300),	one(0177700), "$s", m68000up },
{"sle", 2,	one(0057700),	one(0177700), "$s", m68000up },
{"sls", 2,	one(0051700),	one(0177700), "$s", m68000up },
{"slt", 2,	one(0056700),	one(0177700), "$s", m68000up },
{"smi", 2,	one(0055700),	one(0177700), "$s", m68000up },
{"sne", 2,	one(0053300),	one(0177700), "$s", m68000up },
{"spl", 2,	one(0055300),	one(0177700), "$s", m68000up },
{"st", 2,	one(0050300),	one(0177700), "$s", m68000up },
{"svc", 2,	one(0054300),	one(0177700), "$s", m68000up },
{"svs", 2,	one(0054700),	one(0177700), "$s", m68000up },

{"stop", 4,	one(0047162),	one(0177777), "#w", m68000up },

{"subal", 2,	one(0110700),	one(0170700), "*lAd", m68000up },
{"subaw", 2,	one(0110300),	one(0170700), "*wAd", m68000up },

{"subib", 4,	one(0002000),	one(0177700), "#b$s", m68000up },
{"subiw", 4,	one(0002100),	one(0177700), "#w$s", m68000up },
{"subil", 6,	one(0002200),	one(0177700), "#l$s", m68000up },

{"subqb", 2,	one(0050400),	one(0170700), "Qd%s", m68000up },
{"subqw", 2,	one(0050500),	one(0170700), "Qd%s", m68000up },
{"subql", 2,	one(0050600),	one(0170700), "Qd%s", m68000up },

/* The sub opcode can generate the suba, subi, and subq instructions.  */
{"subb", 2,	one(0050400),	one(0170700), "Qd%s", m68000up },
{"subb", 4,	one(0002000),	one(0177700), "#b$s", m68000up },
{"subb", 2,	one(0110000),	one(0170700), ";bDd", m68000up },
{"subb", 2,	one(0110400),	one(0170700), "Dd~s", m68000up },
{"subw", 2,	one(0050500),	one(0170700), "Qd%s", m68000up },
{"subw", 4,	one(0002100),	one(0177700), "#w$s", m68000up },
{"subw", 2,	one(0110300),	one(0170700), "*wAd", m68000up },
{"subw", 2,	one(0110100),	one(0170700), "*wDd", m68000up },
{"subw", 2,	one(0110500),	one(0170700), "Dd~s", m68000up },
{"subl", 2,	one(0050600),	one(0170700), "Qd%s", m68000up },
{"subl", 6,	one(0002200),	one(0177700), "#l$s", m68000up },
{"subl", 2,	one(0110700),	one(0170700), "*lAd", m68000up },
{"subl", 2,	one(0110200),	one(0170700), "*lDd", m68000up },
{"subl", 2,	one(0110600),	one(0170700), "Dd~s", m68000up },

{"subxb", 2,	one(0110400),	one(0170770), "DsDd", m68000up },
{"subxb", 2,	one(0110410),	one(0170770), "-s-d", m68000up },
{"subxw", 2,	one(0110500),	one(0170770), "DsDd", m68000up },
{"subxw", 2,	one(0110510),	one(0170770), "-s-d", m68000up },
{"subxl", 2,	one(0110600),	one(0170770), "DsDd", m68000up },
{"subxl", 2,	one(0110610),	one(0170770), "-s-d", m68000up },

{"swap", 2,	one(0044100),	one(0177770), "Ds", m68000up },

{"tas", 2,	one(0045300),	one(0177700), "$s", m68000up},

{"trap", 2,	one(0047100),	one(0177760), "Ts", m68000up },

{"trapv", 2,	one(0047166),	one(0177777), "", m68000up },

{"tstb", 2,	one(0045000),	one(0177700), "$b", m68000up },
{"tstw", 2,	one(0045100),	one(0177700), "$w", m68000up },
{"tstl", 2,	one(0045200),	one(0177700), "$l", m68000up },

{"unlk", 2,	one(0047130),	one(0177770), "As", m68000up },
};

const int m68k_numopcodes = sizeof m68k_opcodes / sizeof m68k_opcodes[0];

static int read_buffer(bfd_vma memaddr, uint8_t *myaddr,
	int length, struct disassemble_info *info)
{
	const size_t offset = memaddr - info->insn_memory->address;

	if (length < 1 || info->insn_memory->size < offset + length)
		return -EFAULT;

	memcpy(myaddr, &info->insn_memory->data[offset], length);

	return 0;
}

static void memory_error(int status, bfd_vma memaddr,
	struct disassemble_info *info)
{
#if 0
	fprintf(stderr, "%s: %d %" PRIu64 "\n",		/* FIXME */
		__func__, status, memaddr);
#endif
}

static void print_address(bfd_vma addr, struct disassemble_info *info)
{
	struct m68k_symbol sym = { };

	info->target = addr;

	if (info->symbol)
		sym = info->symbol(info->stream, addr);

	if (sym.s[0])
		info->fprintf_func(info->stream, "%s", sym.s);
	else
		info->fprintf_func(info->stream, "0x%" PRIx64,
			addr & 0xffffff);
}

int m68k_disassemble_instruction(const void *data, size_t size,
	uint32_t address,
	struct m68k_symbol (*symbol)(void *arg, uint32_t address),
	int (*print)(void *arg, const char *fmt, ...),
	void *arg)
{
	const struct insn_memory insn_memory = {
		.size = size,
		.data = data,
		.address = address,
	};
	struct disassemble_info info = {
		.insn_memory = &insn_memory,
		.fprintf_func = print,
		.stream = arg,

		.arch = bfd_mach_m68000,
		.mach = bfd_mach_m68000,

		.read_memory_func = read_buffer,
		.memory_error_func = memory_error,
		.print_address_func = print_address,
		.symbol = symbol,
	};

	return print_insn_m68k(address, &info);
}

int m68k_disassemble_type_target(const void *data, size_t size,
	uint32_t address, enum m68k_insn_type *type, uint32_t *target)
{
	const struct insn_memory insn_memory = {
		.size = size,
		.data = data,
		.address = address,
	};
	struct disassemble_info info = {
		.insn_memory = &insn_memory,
		.fprintf_func = ignore_print,
		.stream = NULL,

		.arch = bfd_mach_m68000,
		.mach = bfd_mach_m68000,

		.read_memory_func = read_buffer,
		.memory_error_func = memory_error,
		.print_address_func = print_address,
	};

	int r = print_insn_m68k(address, &info);

	if (type)
		*type = info.insn_type;

	if (target)
		*target = info.target;

	return r;
}
