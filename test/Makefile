# SPDX-License-Identifier: GPL-2.0

PSGPLAY_test_dir := $(dir $(lastword $(MAKEFILE_LIST)))

PSGPLAY_TEST_SRC := $(wildcard $(PSGPLAY_test_dir)*.c)
PSGPLAY_TEST_OBJ = $(PSGPLAY_TEST_SRC:%.c=%.o)
PSGPLAY_TEST_ELF = $(PSGPLAY_TEST_SRC:%.c=%.elf)
PSGPLAY_TEST_SNDH = $(PSGPLAY_TEST_SRC:%.c=%.sndh)
PSGPLAY_TEST_TUNE := $(shell script/test-tune list $(PSGPLAY_TEST_SRC))
PSGPLAY_TEST_WAVE = $(PSGPLAY_TEST_TUNE:%=%.wave)

PSGPLAY_TEST_SNDH_CFLAGS += $(BASIC_TARGET_CFLAGS) $(CF2149_CFLAGS)	\
	-march=68000 -mpcrel -fpie -nostdlib				\
	-isystem lib/toslibc/include/toslibc -D_TOSLIBC_SOURCE

PSGPLAY_TEST_SNDH_LDFLAGS += --relocatable --gc-sections --strip-all	\
	--entry _sndh -L$(TOSLIBC_lib_dir) --script=$(TOSLIBC_SCRIPT_SNDH_LD)

ALL_OBJ += $(PSGPLAY_TEST_OBJ)
OTHER_CLEAN += $(PSGPLAY_TEST_ELF) $(PSGPLAY_TEST_SNDH) $(PSGPLAY_TEST_WAVE)

ifdef TARGET_CC

PSGPLAY_TEST = ./$(PSGPLAY)
PSGPLAY_TEST_FLAGS = --start 1 --length 60

PSGPLAY_TEST_sndh_tune ?= $(shell echo "$(1)" | sed 's/^.*-\([0-9]\+\).*$$/\1/')
PSGPLAY_TEST_sndh_file ?= $(shell echo "$(1)" | sed 's/-[0-9]\+.*$$//').sndh

define PSGPLAY_TEST_target
$(1).wave: $(PSGPLAY) $$(call PSGPLAY_TEST_sndh_file,$(1).wave)
	$$(QUIET_TEST)$$(PSGPLAY_TEST) -o $$@ $(PSGPLAY_TEST_FLAGS)	\
		-t $$(call PSGPLAY_TEST_sndh_tune,$$@)			\
		   $$(call PSGPLAY_TEST_sndh_file,$$@)
endef

$(foreach t,$(PSGPLAY_TEST_TUNE),$(eval $(call PSGPLAY_TEST_target,$(t))))

.PHONY: test-wave
test-wave: $(PSGPLAY_TEST_WAVE)

$(PSGPLAY_TEST_OBJ): %.o: %.c
	$(QUIET_CC)$(TARGET_CC) $(PSGPLAY_TEST_SNDH_CFLAGS)		\
		$(TARGET_CFLAGS) -c -o $@ $<

PSGPLAY_TEST_SNDH_TOSLIBC_OBJ += $(addprefix $(TOSLIBC_lib_dir),	\
	__divsi3.o							\
	__mulsi3.o							\
	__udivmodsi4.o							\
	__udivsi3.o)

$(PSGPLAY_TEST_ELF): $(TOSLIBC) $(TOSLIBC_SCRIPT_SNDH_LD)		\
	$(PSGPLAY_TEST_SNDH_TOSLIBC_OBJ)
$(PSGPLAY_TEST_ELF): %.elf: %.o
	$(QUIET_LINK)$(TARGET_LD) $(PSGPLAY_TEST_SNDH_LDFLAGS)		\
		$(TARGET_LDFLAGS) -o $@ $< $(PSGPLAY_TEST_SNDH_TOSLIBC_OBJ)
	@chmod a-x $@

$(PSGPLAY_TEST_SNDH): $(TOSLINK)

$(PSGPLAY_TEST_SNDH): %.sndh: %.elf
	$(QUIET_LINK)$(TOSLINK) -o $@ $<

endif
