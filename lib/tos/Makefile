# SPDX-License-Identifier: GPL-2.0

TOS_CFLAGS += $(BASIC_TARGET_CFLAGS) -march=68000 -fno-PIC -nostdlib	\
	-isystem lib/toslibc/include/toslibc -D_TOSLIBC_SOURCE

TOS_LDFLAGS += --orphan-handling=error --discard-all -nostdlib		\
	--no-relax -no-pie --script=script/tos.ld

ALL_OBJ += lib/tos/reset.o lib/tos/sndh.o

ifdef TARGET_CC

lib/tos/reset.o: lib/tos/reset.S
	$(QUIET_AS)$(TARGET_CC) $(TOS_CFLAGS) $(TARGET_CFLAGS) -c -o $@ $<

lib/tos/sndh.o: lib/tos/sndh.c
	$(QUIET_CC)$(TARGET_CC) $(TOS_CFLAGS) $(TARGET_CFLAGS) -c -o $@ $<

TOS_TOSLIBC_OBJ += $(addprefix $(TOSLIBC_lib_dir),			\
	__mulsi3.o							\
	__udivmodsi4.o							\
	__udivsi3.o)

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

LIBPSGPLAY_TOS_HEADER = include/tos/tos.h

$(LIBPSGPLAY_TOS_HEADER): lib/tos/tos
	$(QUIET_GEN)script/tos $< $@

OTHER_CLEAN += $(LIBPSGPLAY_TOS_HEADER)
