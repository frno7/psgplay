# SPDX-License-Identifier: GPL-2.0

CHECK_CFLAGS += -O2 -Wall -march=68000

CHECK_SRC := check/68000.c

CHECK_OBJ += $(patsubst %.c, %.o, $(CHECK_SRC))

$(CHECK_OBJ): %.o : %.c
	$(QUIET_CC)$(CROSS_COMPILE)gcc $(CHECK_CFLAGS) -c -o $@ $<

check/68000.actual: check/68000.o
	$(QUIET_GEN)$(CROSS_COMPILE)objdump -d $^ |			\
		awk '$$2 == "<f>:" { f=1 } f { print }' > $@

.PHONY: check
check: check/68000

.PHONY: check/68000
check/68000: check/68000.actual check/68000.expect
	$(QUIET_CHECK)cmp --quiet $^ ||					\
		(echo "error: $(CROSS_COMPILE)gcc doesn't emit 68000"; exit 1)
