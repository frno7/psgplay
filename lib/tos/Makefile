# SPDX-License-Identifier: GPL-2.0

TOS_CFLAGS += $(CFLAGS) -march=68000 -fno-PIC -nostdlib			\
	-isystem include/toslibc

TOS_LDFLAGS += --orphan-handling=error --discard-all -nostdlib		\
	--no-relax -no-PIE --script=script/tos.ld

ifneq "x$(CROSS_COMPILE)" "x"

lib/tos/reset.o: lib/tos/reset.S
	$(QUIET_AS)$(CROSS_COMPILE)gcc $(TOS_CFLAGS) -c -o $@ $<

lib/tos/sndh.o: lib/tos/sndh.c
	$(QUIET_CC)$(CROSS_COMPILE)gcc $(TOS_CFLAGS) -c -o $@ $<

lib/tos/tos: script/tos.ld script/tos
lib/tos/tos: lib/tos/reset.o lib/tos/sndh.o
	$(QUIET_LINK)$(CROSS_COMPILE)ld $(TOS_LDFLAGS)			\
		-o $@ lib/tos/reset.o lib/tos/sndh.o
	@chmod a-x $@

else

# Use precompiled TOS when CROSS_COMPILE is undefined.
lib/tos/tos:
	@touch $@

endif

include/tos/tos.h: lib/tos/tos
	$(QUIET_GEN)script/tos $< $@
