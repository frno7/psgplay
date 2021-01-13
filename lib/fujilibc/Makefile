# SPDX-License-Identifier: GPL-2.0

ifneq "x$(CROSS_COMPILE)" "x"

FUJILIBC_CFLAGS +=  $(DEP_CFLAGS) -march=68000 -fno-PIC -nostdlib	\
	-ffunction-sections -fdata-sections -ffreestanding		\
	-isystem include/fujilibc -D_FUJILIBC_SOURCE

FUJILIBC_S_SRC +=							\
	lib/fujilibc/crt0.S						\
	lib/fujilibc/bios.S						\
	lib/fujilibc/gemdos.S						\
	lib/fujilibc/xbios.S						\
	lib/fujilibc/xgemdos.S						\
	lib/fujilibc/__mulsi3.S

FUJILIBC_C_SRC +=							\
	lib/fujilibc/aes.c						\
	lib/fujilibc/close.c						\
	lib/fujilibc/ctype.c						\
	lib/fujilibc/dprintf.c						\
	lib/fujilibc/errno.c						\
	lib/fujilibc/exit.c						\
	lib/fujilibc/free.c						\
	lib/fujilibc/lseek.c						\
	lib/fujilibc/malloc.c						\
	lib/fujilibc/memcpy.c						\
	lib/fujilibc/memset.c						\
	lib/fujilibc/open.c						\
	lib/fujilibc/printf.c						\
	lib/fujilibc/ptermres.c						\
	lib/fujilibc/putchar.c						\
	lib/fujilibc/puts.c						\
	lib/fujilibc/read.c						\
	lib/fujilibc/snprintf.c						\
	lib/fujilibc/start.c						\
	lib/fujilibc/strcmp.c						\
	lib/fujilibc/strerror.c						\
	lib/fujilibc/strlen.c						\
	lib/fujilibc/strncmp.c						\
	lib/fujilibc/strncpy.c						\
	lib/fujilibc/strntoimax.c					\
	lib/fujilibc/strntoumax.c					\
	lib/fujilibc/strsplit.c						\
	lib/fujilibc/strtoimax.c					\
	lib/fujilibc/strtol.c						\
	lib/fujilibc/strtoll.c						\
	lib/fujilibc/strtoul.c						\
	lib/fujilibc/strtoull.c						\
	lib/fujilibc/strtoumax.c					\
	lib/fujilibc/supexecarg.c					\
	lib/fujilibc/system-variable.c					\
	lib/fujilibc/vdi.c						\
	lib/fujilibc/vdprintf.c						\
	lib/fujilibc/vprintf.c						\
	lib/fujilibc/vsnprintf.c					\
	lib/fujilibc/write.c						\
	lib/fujilibc/xbra.c						\
	lib/fujilibc/__divsi3.c						\
	lib/fujilibc/__modsi3.c						\
	lib/fujilibc/__muldi3.c						\
	lib/fujilibc/__udivdi3.c					\
	lib/fujilibc/__udivmoddi4.c					\
	lib/fujilibc/__udivmodsi4.c					\
	lib/fujilibc/__udivsi3.c					\
	lib/fujilibc/__umoddi3.c

FUJILIBC_S_OBJ += $(patsubst %.S, %.o, $(FUJILIBC_S_SRC))
FUJILIBC_C_OBJ += $(patsubst %.c, %.o, $(FUJILIBC_C_SRC))

FUJILIBC_OBJ += $(FUJILIBC_S_OBJ) $(FUJILIBC_C_OBJ)

$(FUJILIBC_S_OBJ): %.o : %.S
	$(QUIET_CC)$(CROSS_COMPILE)gcc $(FUJILIBC_CFLAGS) -c -o $@ $<

$(FUJILIBC_C_OBJ): %.o : %.c
	$(QUIET_CC)$(CROSS_COMPILE)gcc $(FUJILIBC_CFLAGS) -c -o $@ $<

endif
