# SPDX-License-Identifier: GPL-2.0

PSGPLAY_test_dir := $(dir $(lastword $(MAKEFILE_LIST)))

PSGPLAY_TEST_SRC := $(wildcard $(PSGPLAY_test_dir)*.c)
PSGPLAY_TEST_OBJ = $(PSGPLAY_TEST_SRC:%.c=%.o)
PSGPLAY_TEST_SNDH = $(PSGPLAY_TEST_SRC:%.c=%.sndh)

SNDH_CFLAGS += $(BASIC_TARGET_CFLAGS) $(CF2149_CFLAGS)			\
	-march=68000 -mpcrel -fpie -nostdlib				\
	-isystem lib/toslibc/include/toslibc -D_TOSLIBC_SOURCE

SNDH_LDFLAGS += --orphan-handling=error --discard-all -nostdlib		\
	--no-relax --script=script/sndh.ld

ALL_OBJ += $(PSGPLAY_TEST_OBJ)
OTHER_CLEAN += $(PSGPLAY_TEST_SNDH)

ifdef TARGET_CC

$(PSGPLAY_TEST_OBJ): %.o: %.c
	$(QUIET_CC)$(TARGET_CC) $(SNDH_CFLAGS) $(TARGET_CFLAGS) -c -o $@ $<

SNDH_TOSLIBC_OBJ += $(addprefix $(TOSLIBC_lib_dir),			\
	__mulsi3.o							\
	__udivmodsi4.o							\
	__udivsi3.o)

$(PSGPLAY_TEST_SNDH): $(SNDH_TOSLIBC_OBJ)
$(PSGPLAY_TEST_SNDH): script/sndh.ld
$(PSGPLAY_TEST_SNDH): %.sndh: %.o
	$(QUIET_LINK)$(TARGET_LD) $(SNDH_LDFLAGS) $(TARGET_LDFLAGS)	\
		-o $@ $< $(SNDH_TOSLIBC_OBJ)
	@chmod a-x $@

endif
