# SPDX-License-Identifier: GPL-2.0

PSGPLAY_test_dir := $(dir $(lastword $(MAKEFILE_LIST)))

PSGPLAY_TESTS :=							\
	dmapitch							\
	maxamp								\
	psgpitch							\
	sndhtimera							\
	sndhtimerb							\
	sndhtimerc							\
	sndhtimerd							\
	sndhtimervbl							\
	tempo
PSGPLAY_TEST_INCLUDE :=							\
	$(addsuffix .h,$(addprefix include/test/,$(PSGPLAY_TESTS)))
PSGPLAY_TEST_SRC :=							\
	$(addsuffix .c,$(addprefix $(PSGPLAY_test_dir),$(PSGPLAY_TESTS)))
PSGPLAY_TEST_OBJ = $(PSGPLAY_TEST_SRC:%.c=%.o)
PSGPLAY_TEST_ELF = $(PSGPLAY_TEST_SRC:%.c=%.elf)
PSGPLAY_TEST_SNDH = $(PSGPLAY_TEST_SRC:%.c=%.sndh)
PSGPLAY_TEST_TUNE := $(addprefix $(PSGPLAY_test_dir),$(notdir		\
	$(shell script/test-tune list $(PSGPLAY_TEST_INCLUDE))))
PSGPLAY_TEST_WAVE = $(PSGPLAY_TEST_TUNE:%=%.wave)
PSGPLAY_TEST_SVG = $(PSGPLAY_TEST_TUNE:%=%.svg)
PSGPLAY_TEST_PNG = $(PSGPLAY_TEST_TUNE:%=%.png)
PSGPLAY_TEST_REPORT = $(PSGPLAY_TEST_TUNE:%=%.report)
PSGPLAY_TEST_VERIFY = $(PSGPLAY_TEST_SRC:%.c=%-verify)
PSGPLAY_TEST_VERIFY_OBJ = $(PSGPLAY_TEST_VERIFY:%=%.o)
PSGPLAY_TEST_VERIFY_TUNE = $(addprefix verify-,$(notdir $(PSGPLAY_TEST_TUNE)))
PSGPLAY_TEST_REPORT_TUNE = $(addprefix report-,$(notdir $(PSGPLAY_TEST_TUNE)))

PSGPLAY_TEST_SNDH_CFLAGS += $(BASIC_TARGET_CFLAGS) $(CF2149_CFLAGS)	\
	-march=68000 -mpcrel -fpie -nostdlib				\
	-Ilib/toslibc/include -isystem lib/toslibc/include/toslibc	\
	-D_TOSLIBC_SOURCE

PSGPLAY_TEST_SNDH_LDFLAGS += --relocatable --gc-sections --strip-all	\
	--entry _sndh -L$(TOSLIBC_lib_dir) --script=$(TOSLIBC_SCRIPT_SNDH_LD)

ALL_OBJ += $(PSGPLAY_TEST_OBJ) $(PSGPLAY_TEST_VERIFY_OBJ)
OTHER_CLEAN += $(PSGPLAY_TEST_ELF) $(PSGPLAY_TEST_SNDH)			\
	$(PSGPLAY_TEST_WAVE) $(PSGPLAY_TEST_SVG) $(PSGPLAY_TEST_PNG)	\
	$(PSGPLAY_TEST_REPORT) $(PSGPLAY_TEST_VERIFY)

PSGPLAY_TEST = ./$(PSGPLAY)
PSGPLAY_TEST_FLAGS = --length 63

ifdef TARGET_CC

PSGPLAY_TEST_sndh_tune ?= $(shell echo "$(1)" | sed 's/^.*-\([0-9]\+\).*$$/\1/')
PSGPLAY_TEST_sndh_base ?= $(shell echo "$(1)" | sed 's/-[0-9]\+.*$$//')

define PSGPLAY_TEST_target
$(1).wave: $$(call PSGPLAY_TEST_sndh_base,$(1)).sndh $(PSGPLAY)
	$$(QUIET_TEST)$$(PSGPLAY_TEST) -o $$@ $(PSGPLAY_TEST_FLAGS)	\
		-t $$(call PSGPLAY_TEST_sndh_tune,$$@) $$<

$(1).svg: $$(call PSGPLAY_TEST_sndh_base,$(1))-verify $(1).wave
	$$(QUIET_GEN)$$< graph -t $$(call PSGPLAY_TEST_sndh_tune,$$@)	\
		-o $$@ $(1).wave

$(1).report: $$(call PSGPLAY_TEST_sndh_base,$(1))-verify $(1).wave
	$$(QUIET_GEN)$$< report -t $$(call PSGPLAY_TEST_sndh_tune,$$@)	\
		-o $$@ $(1).wave

.PHONY: verify-$(notdir $(1))
verify-$(notdir $(1)): $$(call PSGPLAY_TEST_sndh_base,$(1))-verify $(1).wave
	$$(QUIET_VERIFY)$$< verify -t $$(call PSGPLAY_TEST_sndh_tune,$$@) \
		$(1).wave

.PHONY: report-$(notdir $(1))
report-$(notdir $(1)): $(1).report
	@cat $$^
endef

$(foreach t,$(PSGPLAY_TEST_TUNE),$(eval $(call PSGPLAY_TEST_target,$(t))))

.PHONY: test-sndh
test-sndh: $(PSGPLAY_TEST_SNDH)

.PHONY: test-wave
test-wave: $(PSGPLAY_TEST_WAVE)

.PHONY: test-svg
test-svg: $(PSGPLAY_TEST_SVG)

.PHONY: test-png
test-png: $(PSGPLAY_TEST_PNG)

.PHONY: test-report
test-report: $(PSGPLAY_TEST_REPORT)

.PHONY: verify
verify: $(PSGPLAY_TEST_VERIFY_TUNE)

.PHONY: report
report: $(PSGPLAY_TEST_REPORT)
	@cat $^

define PSGPLAY_VERIFY_target
.PHONY: verify-$(1)
verify-$(1): $(filter verify-$(1)-%,$(PSGPLAY_TEST_VERIFY_TUNE))

.PHONY: report-$(1)
report-$(1): $(filter $(addprefix $(PSGPLAY_test_dir),$(1))-%,$(PSGPLAY_TEST_REPORT))
	@cat $$^
endef

$(foreach t,$(PSGPLAY_TESTS),$(eval $(call PSGPLAY_VERIFY_target,$(t))))

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

PSGPLAY_TEST_VERIFY_CFLAGS = $(BASIC_HOST_CFLAGS) $(HOST_CFLAGS)	\
	-Ilib/toslibc/include

PSGPLAY_TEST_VERIFY_LDFLAGS = $(PSGPLAY_TEST_VERIFY_CFLAGS) -lm

$(PSGPLAY_TEST_VERIFY_OBJ): %.o: %.c
	$(QUIET_CC)$(HOST_CC) $(PSGPLAY_TEST_VERIFY_CFLAGS) -c -o $@ $<

$(PSGPLAY_TEST_VERIFY): $(PSGPLAY_LIB_TEST_OBJ)
$(PSGPLAY_TEST_VERIFY): %: %.o
	$(QUIET_LINK)$(HOST_CC) -o $@ $^ $(PSGPLAY_TEST_VERIFY_LDFLAGS)

SVGTOPNG = rsvg-convert
SVGTOPNG_FLAGS = --background-color=white

$(PSGPLAY_TEST_PNG): %.png: %.svg
	$(QUIET_GEN)$(SVGTOPNG) -o $@ $(SVGTOPNG_FLAGS) $<

endif # TARGET_CC

SNDH_ARCHIVE_SUITE = $(addprefix $(PSGPLAY_test_dir),archive.suite)

ifdef SNDH_ARCHIVE_DIR

.PHONY: $(SNDH_ARCHIVE_SUITE)
$(SNDH_ARCHIVE_SUITE):
	$(QUIET_GEN)script/archive-suite enum $(PSGPLAY_TEST) $(SNDH_ARCHIVE_DIR) $@

SNDH_ARCHIVE_SUITE_LENGTH := $(shell wc -l $(SNDH_ARCHIVE_SUITE) | cut -f1 -d' ')

define SNDH_ARCHIVE_target_file_tune
test/archive-$$(SNDH_ARCHIVE_TAG)$(1)-$(2).sha256:			\
	FILE=$(shell script/archive-suite name $(1) $(SNDH_ARCHIVE_SUITE))
test/archive-$$(SNDH_ARCHIVE_TAG)$(1)-$(2).sha256:
	@echo '  SHA256  $$(@) $(PSGPLAY_TEST) $(PSGPLAY_TEST_FLAGS) $$(FILE)'
	@$(PSGPLAY_TEST) --quiet -t $(2) $$(PSGPLAY_TEST_FLAGS)		\
		-o $$(@:%.sha256=%.wave) "$$(SNDH_ARCHIVE_DIR)/$$(FILE)"
	@sha256sum $$(@:%.sha256=%.wave) | sed 's# .*$$$$# $(2) $$(FILE)#' \
		>$$(@:%.sha256=%.tmp)
	@rm $$(@:%.sha256=%.wave)
	@mv $$(@:%.sha256=%.tmp) $$@

SNDH_ARCHIVE_SUITE_SHA256 += test/archive-$$(SNDH_ARCHIVE_TAG)$(1)-$(2).sha256

.PHONY: verify-archive-sha256-$(1)-$(2)-sha256
verify-archive-sha256-$(1)-$(2)-sha256:					\
	FILE=$(shell script/archive-suite name $(1) $(SNDH_ARCHIVE_SUITE))
verify-archive-sha256-$(1)-$(2)-sha256:
	@echo '  VERIFY  $$(FILE) $(2) $$(FILE)'
	@[ "$$(SNDH_ARCHIVE_TAG)"x = ""x ]
	@[ "$$(SNDH_ARCHIVE_TAG_A)"x != "$$(SNDH_ARCHIVE_TAG_B)"x ]
	@cmp --quiet test/archive-$$(SNDH_ARCHIVE_TAG_A)$(1)-$(2).sha256 \
		     test/archive-$$(SNDH_ARCHIVE_TAG_B)$(1)-$(2).sha256

SNDH_ARCHIVE_SUITE_VERIFY_SHA256 += verify-archive-sha256-$(1)-$(2)-sha256
endef

define SNDH_ARCHIVE_target_file
$(foreach t,$(shell script/archive-suite tune-seq $(1) $(SNDH_ARCHIVE_SUITE)),\
	$(eval $(call SNDH_ARCHIVE_target_file_tune,$(1),$(t))))
endef

$(foreach n,$(shell seq $(SNDH_ARCHIVE_SUITE_LENGTH)),			\
	$(eval $(call SNDH_ARCHIVE_target_file,$(n))))

.PHONY: test-archive
test-archive: $(SNDH_ARCHIVE_SUITE_SHA256)

.PHONY: verify-archive
verify-archive: $(SNDH_ARCHIVE_SUITE_VERIFY_SHA256)

.PHONY: verify-archive-old-new
verify-archive-old-new: SNDH_ARCHIVE_TAG_A=old-
verify-archive-old-new: SNDH_ARCHIVE_TAG_B=new-
verify-archive-old-new: verify-archive

else

.PHONY: $(SNDH_ARCHIVE_SUITE)
$(SNDH_ARCHIVE_SUITE):
	$(error SNDH_ARCHIVE_DIR undefined)

endif # SNDH_ARCHIVE_DIR
