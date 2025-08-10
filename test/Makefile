# SPDX-License-Identifier: GPL-2.0

PSGPLAY_test_dir := $(dir $(lastword $(MAKEFILE_LIST)))

PSGPLAY_TEST_SRC := $(wildcard $(PSGPLAY_test_dir)*.c)
PSGPLAY_TEST_OBJ = $(PSGPLAY_TEST_SRC:%.c=%.o)
PSGPLAY_TEST_ELF = $(PSGPLAY_TEST_SRC:%.c=%.elf)
PSGPLAY_TEST_SNDH = $(PSGPLAY_TEST_SRC:%.c=%.sndh)

SNDH_CFLAGS += $(BASIC_TARGET_CFLAGS) $(CF2149_CFLAGS)			\
	-march=68000 -mpcrel -fpie -nostdlib				\
	-isystem lib/toslibc/include/toslibc -D_TOSLIBC_SOURCE

SNDH_LDFLAGS += --relocatable --gc-sections --strip-all --entry _sndh	\
	-L$(TOSLIBC_lib_dir) --script=$(TOSLIBC_SCRIPT_SNDH_LD)

ALL_OBJ += $(PSGPLAY_TEST_OBJ)
OTHER_CLEAN += $(PSGPLAY_TEST_ELF) $(PSGPLAY_TEST_SNDH)

ifdef TARGET_CC

$(PSGPLAY_TEST_OBJ): %.o: %.c
	$(QUIET_CC)$(TARGET_CC) $(SNDH_CFLAGS) $(TARGET_CFLAGS) -c -o $@ $<

SNDH_TOSLIBC_OBJ += $(addprefix $(TOSLIBC_lib_dir),			\
	__divsi3.o							\
	__mulsi3.o							\
	__udivmodsi4.o							\
	__udivsi3.o)

$(PSGPLAY_TEST_ELF): $(TOSLIBC) $(TOSLIBC_SCRIPT_SNDH_LD) $(SNDH_TOSLIBC_OBJ)
$(PSGPLAY_TEST_ELF): %.elf: %.o
	$(QUIET_LINK)$(TARGET_LD) $(SNDH_LDFLAGS) $(TARGET_LDFLAGS)	\
		-o $@ $< $(SNDH_TOSLIBC_OBJ)
	@chmod a-x $@

$(PSGPLAY_TEST_SNDH): $(TOSLINK)

$(PSGPLAY_TEST_SNDH): %.sndh: %.elf
	$(QUIET_LINK)$(TOSLINK) -o $@ $<

endif
