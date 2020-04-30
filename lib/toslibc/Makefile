# SPDX-License-Identifier: GPL-2.0

ifneq "x$(CROSS_COMPILE)" "x"

TOSLIBC_CFLAGS +=  $(CFLAGS) -march=68000 -fno-PIC -nostdlib		\
	-ffunction-sections -fdata-sections -isystem include/toslibc

TOSLIBC_S_SRC +=							\
	lib/toslibc/__mulsi3.S

TOSLIBC_C_SRC +=							\
	lib/toslibc/__udivmodsi4.c					\
	lib/toslibc/__udivsi3.c

TOSLIBC_S_OBJ += $(patsubst %.S, %.o, $(TOSLIBC_S_SRC))
TOSLIBC_C_OBJ += $(patsubst %.c, %.o, $(TOSLIBC_C_SRC))

TOSLIBC_OBJ += $(TOSLIBC_S_OBJ) $(TOSLIBC_C_OBJ)

$(TOSLIBC_S_OBJ): %.o : %.S
	$(QUIET_CC)$(CROSS_COMPILE)gcc $(TOSLIBC_CFLAGS) -c -o $@ $<

$(TOSLIBC_C_OBJ): %.o : %.c
	$(QUIET_CC)$(CROSS_COMPILE)gcc $(TOSLIBC_CFLAGS) -c -o $@ $<

endif
