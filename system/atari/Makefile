# SPDX-License-Identifier: GPL-2.0

PRG_CFLAGS += $(DEP_CFLAGS) -march=68000 -fno-PIC -nostdlib		\
	-ffunction-sections -fdata-sections -isystem include/fujilibc	\
	-D_TOSLIBC_SOURCE

PRG_LDFLAGS += --relocatable --gc-sections --strip-all --entry _start	\
	--script=script/prg.ld

PRG_VLINKFLAGS += -b ataritos -tos-fastload -e _start

PSGPLAY_TOS := PSGPLAY.TOS
PSGPLAY_PRG_OBJ := system/atari/PSGPLAY.PRG.o

SYSTEM_ATARI_SRC := $(wildcard system/atari/*.c)
SYSTEM_ATARI_OBJ := $(patsubst %.c, %.o, $(SYSTEM_ATARI_SRC))

$(SYSTEM_ATARI_OBJ): %.o : %.c
	$(QUIET_CC)$(CROSS_COMPILE)gcc $(PRG_CFLAGS) -c -o $@ $<

system/atari/ice_decrunch_inplace_.o: system/atari/ice_decrunch_inplace_.S
	$(QUIET_CC)$(CROSS_COMPILE)gcc $(PRG_CFLAGS) -c -o $@ $<

system/atari/fifo.o: lib/internal/fifo.c
	$(QUIET_CC)$(CROSS_COMPILE)gcc $(PRG_CFLAGS) -c -o $@ $<

system/atari/string.o: lib/internal/string.c
	$(QUIET_CC)$(CROSS_COMPILE)gcc $(PRG_CFLAGS) -c -o $@ $<

system/atari/ice.o: lib/ice/ice.c
	$(QUIET_CC)$(CROSS_COMPILE)gcc $(PRG_CFLAGS) -c -o $@ $<

system/atari/sndh.o: lib/psgplay/sndh.c
	$(QUIET_CC)$(CROSS_COMPILE)gcc $(PRG_CFLAGS) -c -o $@ $<

system/atari/load.o: lib/text/load.c
	$(QUIET_CC)$(CROSS_COMPILE)gcc $(PRG_CFLAGS) -c -o $@ $<

system/atari/main.o: lib/text/main.c
	$(QUIET_CC)$(CROSS_COMPILE)gcc $(PRG_CFLAGS) -c -o $@ $<

system/atari/mode.o: lib/text/mode.c
	$(QUIET_CC)$(CROSS_COMPILE)gcc $(PRG_CFLAGS) -c -o $@ $<

system/atari/vt52.o: lib/vt/vt52.c
	$(QUIET_CC)$(CROSS_COMPILE)gcc $(PRG_CFLAGS) -c -o $@ $<

system/atari/vt.o: lib/vt/vt.c
	$(QUIET_CC)$(CROSS_COMPILE)gcc $(PRG_CFLAGS) -c -o $@ $<

SYSTEM_ATARI_SNDH_OBJ :=						\
	system/atari/fifo.o						\
	system/atari/ice.o						\
	system/atari/ice_decrunch_inplace_.o				\
	system/atari/load.o						\
	system/atari/main.o						\
	system/atari/mode.o						\
	system/atari/sndh.o						\
	system/atari/string.o						\
	system/atari/vt.o						\
	system/atari/vt52.o

$(PSGPLAY_PRG_OBJ): check/68000
$(PSGPLAY_PRG_OBJ): $(TOSLIBC_OBJ) $(SYSTEM_ATARI_OBJ)			\
		$(SYSTEM_ATARI_SNDH_OBJ)
	$(QUIET_LINK)$(CROSS_COMPILE)ld $(PRG_LDFLAGS) -o $@		\
		$(TOSLIBC_OBJ) $(SYSTEM_ATARI_OBJ)			\
		$(SYSTEM_ATARI_SNDH_OBJ)

$(PSGPLAY_TOS): $(PSGPLAY_PRG_OBJ)
	$(QUIET_LINK)vlink $(PRG_VLINKFLAGS) -o $@ $^
