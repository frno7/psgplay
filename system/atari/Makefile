# SPDX-License-Identifier: GPL-2.0

PRG_CFLAGS += $(SOME_CFLAGS) -march=68000 -fno-PIC -nostdlib		\
	-ffunction-sections -fdata-sections -isystem include/toslibc	\
	-D_TOSLIBC_SOURCE

PRG_LDFLAGS += --relocatable --gc-sections --strip-all --entry _start	\
	--script=script/prg.ld

PRG_VLINKFLAGS += -b ataritos -tos-fastload -e _start

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

$(PSGPLAY_PRG_OBJ): $(TOSLIBC_OBJ) $(SYSTEM_ATARI_OBJ)
	$(QUIET_LINK)$(TARGET_LD) $(PRG_LDFLAGS) $(TARGET_LDFLAGS) -o $@ \
		$(TOSLIBC_OBJ) $(SYSTEM_ATARI_OBJ)

$(PSGPLAY_TOS): $(PSGPLAY_PRG_OBJ)
	$(QUIET_LINK)vlink $(PRG_VLINKFLAGS) -o $@ $^

OTHER_CLEAN += $(PSGPLAY_TOS)
