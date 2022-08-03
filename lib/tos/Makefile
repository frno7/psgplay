# SPDX-License-Identifier: GPL-2.0

TOS_CFLAGS += $(SOME_CFLAGS) -march=68000 -fno-PIC -nostdlib		\
	-isystem include/toslibc -D_TOSLIBC_SOURCE

TOS_LDFLAGS += --orphan-handling=error --discard-all -nostdlib		\
	--no-relax -no-pie --script=script/tos.ld

ALL_OBJ += lib/tos/reset.o lib/tos/sndh.o

ifneq "x$(TARGET_CC)" "x"

lib/tos/reset.o: lib/tos/reset.S
	$(QUIET_AS)$(TARGET_CC) $(TOS_CFLAGS) $(TARGET_CFLAGS) -c -o $@ $<

lib/tos/sndh.o: lib/tos/sndh.c
	$(QUIET_CC)$(TARGET_CC) $(TOS_CFLAGS) $(TARGET_CFLAGS) -c -o $@ $<

TOS_TOSLIBC_OBJ +=							\
	lib/toslibc/__mulsi3.o						\
	lib/toslibc/__udivmodsi4.o					\
	lib/toslibc/__udivsi3.o

lib/tos/tos: $(TOS_TOSLIBC_OBJ)
lib/tos/tos: script/tos.ld script/tos
lib/tos/tos: lib/tos/reset.o lib/tos/sndh.o
	$(QUIET_LINK)$(TARGET_LD) $(TOS_LDFLAGS) $(TARGET_LDFLAGS)	\
		-o $@ lib/tos/reset.o lib/tos/sndh.o $(TOS_TOSLIBC_OBJ)
	@chmod a-x $@

else

# Use precompiled TOS when TARGET_CC is undefined.
lib/tos/tos:
	@touch $@

endif

include/tos/tos.h: lib/tos/tos
	$(QUIET_GEN)script/tos $< $@
