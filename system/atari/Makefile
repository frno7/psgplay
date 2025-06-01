# SPDX-License-Identifier: GPL-2.0

PRG_CFLAGS += $(BASIC_TARGET_CFLAGS) $(CF2149_CFLAGS)			\
	-march=68000 -fno-PIC -nostdlib					\
	-ffunction-sections -fdata-sections				\
	-isystem lib/toslibc/include/toslibc -D_TOSLIBC_SOURCE

PRG_LDFLAGS += --relocatable --gc-sections --strip-all --entry _start	\
	-L$(TOSLIBC_lib_dir) --script=$(TOSLIBC_SCRIPT_PRG_LD)

PSGPLAY_TOS := PSGPLAY.TOS
PSGPLAY_PRG_OBJ := system/atari/PSGPLAY.PRG.o

SYSTEM_ATARI_SRC :=							\
	lib/internal/fifo.c						\
	lib/internal/string.c						\
	lib/ice/ice.c							\
	lib/psgplay/sndh.c						\
	lib/text/load.c							\
	lib/text/main.c							\
	lib/text/mode.c							\
	lib/unicode/atari.c						\
	lib/unicode/utf8.c						\
	lib/vt/vt52.c							\
	lib/vt/vt.c							\
	system/atari/clock.c						\
	system/atari/file.c						\
	system/atari/ice_decrunch_inplace.c				\
	system/atari/ice_decrunch_inplace_.S				\
	system/atari/model.c						\
	system/atari/option.c						\
	system/atari/print.c						\
	system/atari/psg.c						\
	system/atari/psgplay.c						\
	system/atari/timer.c

define SYSTEM_ATARI_target
SYSTEM_ATARI_OBJ += system/atari/$(notdir $(basename $(1))).o
system/atari/$(notdir $(basename $(1))).o: $(1)
	$$(QUIET_CC)$$(TARGET_CC) $$(PRG_CFLAGS) $$(TARGET_CFLAGS) -c -o $$@ $$<
endef

$(foreach f,$(SYSTEM_ATARI_SRC),$(eval $(call SYSTEM_ATARI_target,$(f))))

ALL_OBJ += $(SYSTEM_ATARI_OBJ) $(PSGPLAY_PRG_OBJ)

$(PSGPLAY_PRG_OBJ): $(TOSLIBC) $(TOSLIBC_SCRIPT_PRG_LD) $(SYSTEM_ATARI_OBJ)
	$(QUIET_LINK)$(TARGET_LD) $(PRG_LDFLAGS) $(TARGET_LDFLAGS) -o $@ \
		$(SYSTEM_ATARI_OBJ) $(TOSLIBC)

$(PSGPLAY_TOS): $(TOSLINK)

$(PSGPLAY_TOS): $(PSGPLAY_PRG_OBJ)
	$(QUIET_LINK)$(TOSLINK) -o $@ $<

OTHER_CLEAN += $(PSGPLAY_TOS)
