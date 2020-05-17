# SPDX-License-Identifier: GPL-2.0

ifneq "x$(CROSS_COMPILE)" "x"

TOSLIBC_CFLAGS +=  $(DEP_CFLAGS) -march=68000 -fno-PIC -nostdlib	\
	-ffunction-sections -fdata-sections -isystem include/toslibc	\
	-D_TOSLIBC_SOURCE

TOSLIBC_S_SRC +=							\
	lib/toslibc/crt0.S						\
	lib/toslibc/bios.S						\
	lib/toslibc/gemdos.S						\
	lib/toslibc/xbios.S						\
	lib/toslibc/__mulsi3.S

TOSLIBC_C_SRC +=							\
	lib/toslibc/close.c						\
	lib/toslibc/ctype.c						\
	lib/toslibc/dprintf.c						\
	lib/toslibc/errno.c						\
	lib/toslibc/exit.c						\
	lib/toslibc/free.c						\
	lib/toslibc/lseek.c						\
	lib/toslibc/malloc.c						\
	lib/toslibc/memcpy.c						\
	lib/toslibc/memset.c						\
	lib/toslibc/open.c						\
	lib/toslibc/printf.c						\
	lib/toslibc/putchar.c						\
	lib/toslibc/puts.c						\
	lib/toslibc/read.c						\
	lib/toslibc/snprintf.c						\
	lib/toslibc/start.c						\
	lib/toslibc/strcmp.c						\
	lib/toslibc/strlen.c						\
	lib/toslibc/strncmp.c						\
	lib/toslibc/strncpy.c						\
	lib/toslibc/strntoimax.c					\
	lib/toslibc/strntoumax.c					\
	lib/toslibc/strsplit.c						\
	lib/toslibc/strtoimax.c						\
	lib/toslibc/strtol.c						\
	lib/toslibc/strtoll.c						\
	lib/toslibc/strtoul.c						\
	lib/toslibc/strtoull.c						\
	lib/toslibc/strtoumax.c						\
	lib/toslibc/supexecarg.c					\
	lib/toslibc/system-variable.c					\
	lib/toslibc/vdprintf.c						\
	lib/toslibc/vprintf.c						\
	lib/toslibc/vsnprintf.c						\
	lib/toslibc/write.c						\
	lib/toslibc/__divsi3.c						\
	lib/toslibc/__modsi3.c						\
	lib/toslibc/__muldi3.c						\
	lib/toslibc/__udivdi3.c						\
	lib/toslibc/__udivmoddi4.c					\
	lib/toslibc/__udivmodsi4.c					\
	lib/toslibc/__udivsi3.c						\
	lib/toslibc/__umoddi3.c

TOSLIBC_S_OBJ += $(patsubst %.S, %.o, $(TOSLIBC_S_SRC))
TOSLIBC_C_OBJ += $(patsubst %.c, %.o, $(TOSLIBC_C_SRC))

TOSLIBC_OBJ += $(TOSLIBC_S_OBJ) $(TOSLIBC_C_OBJ)

$(TOSLIBC_S_OBJ): %.o : %.S
	$(QUIET_CC)$(CROSS_COMPILE)gcc $(TOSLIBC_CFLAGS) -c -o $@ $<

$(TOSLIBC_C_OBJ): %.o : %.c
	$(QUIET_CC)$(CROSS_COMPILE)gcc $(TOSLIBC_CFLAGS) -c -o $@ $<

endif
