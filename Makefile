# SPDX-License-Identifier: GPL-2.0
#
# Do "make help" for targets and options.

CFLAGS += -g -O2 -Wall -Iinclude -D_GNU_SOURCE

LIBS += -lm

ifeq "$(S)" "1"
CFLAGS += -fsanitize=address -fsanitize=leak -fsanitize=undefined	\
	  -fsanitize-address-use-after-scope -fstack-check
endif

ifeq "$(ALSA)" "1"
CFLAGS += -DHAVE_ALSA
LIBS += -lasound
endif

ALL_CFLAGS += $(CFLAGS) $(BASIC_CFLAGS)

TOS_CFLAGS += -march=68000 -fno-PIC -O2 -Wall -Iinclude -nostdlib

.PHONY: all
all: tool

PSGPLAY := tool/psgplay

.PHONY: tool
tool: $(PSGPLAY)

include lib/Makefile

ifneq "x$(CROSS_COMPILE)" "x"
tos/reset.o: tos/reset.S
	$(QUIET_AS)$(CROSS_COMPILE)gcc $(TOS_CFLAGS) -c -o $@ $<
tos/sndh.o: tos/sndh.c
	$(QUIET_CC)$(CROSS_COMPILE)gcc $(TOS_CFLAGS) -c -o $@ $<
tos/tos: script/tos.ld script/tos
tos/tos: tos/reset.o tos/sndh.o
	$(QUIET_LINK)$(CROSS_COMPILE)ld --orphan-handling=error		\
		--discard-all -nostdlib --no-relax -no-PIE		\
		--script=script/tos.ld -o $@ tos/reset.o tos/sndh.o
	@chmod a-x $@
else
tos/tos:
	@touch $@
endif
include/tos/tos.h: tos/tos
	$(QUIET_GEN)script/tos $< $@

VER := tool/version.c
SRC := $(filter-out $(VER), $(wildcard tool/*.c))			\
	$(ATARI_SRC) $(M68K_SRC) $(VER)
OBJ = $(patsubst %.c, %.o, $(SRC))

$(PSGPLAY): $(OBJ)
	$(QUIET_LINK)$(CC) $(ALL_CFLAGS) -o $@ $^ $(LIBS)

$(OBJ): %.o : %.c
	$(QUIET_CC)$(CC) $(ALL_CFLAGS) -c -o $@ $<

.PHONY: $(shell script/version $(VER))
$(VER):
	@script/version $@

.PHONY: clean
clean:
	$(QUIET_RM)$(RM) -f */*.o* */*/*.o* include/tos/tos.h		\
		GPATH GRTAGS GTAGS 					\
		$(M68K_GEN_H) $(M68K_GEN_C) $(VER) $(PSGPLAY) $(M68KMAKE)

.PHONY: gtags
gtags:
	gtags

.PHONY: help
help:
	@echo "Targets:"
	@echo "  all            - compile the PSG player (default)"
	@echo "  clean          - remove generated files"
	@echo
	@echo "Options:"
	@echo "  V              - set to 1 to compile verbosely"
	@echo "  S              - set to 1 for sanitation checks"
	@echo "  ALSA           - set to 1 to support ALSA for Linux"
	@echo "  CROSS_COMPILE  - set m68k cross compiler to use to build the TOS stub"
	@echo
	@echo "Example:"
	@echo "  make ALSA=1 CROSS_COMPILE=m68k-unknown-linux-gnu-"

V             = @
Q             = $(V:1=)
QUIET_AR      = $(Q:@=@echo    '  AR      '$@;)
QUIET_AS      = $(Q:@=@echo    '  AS      '$@;)
QUIET_CC      = $(Q:@=@echo    '  CC      '$@;)
QUIET_GEN     = $(Q:@=@echo    '  GEN     '$@;)
QUIET_LINK    = $(Q:@=@echo    '  LD      '$@;)
QUIET_TEST    = $(Q:@=@echo    '  TEST    '$@;)
QUIET_RM      = $(Q:@=@echo    '  RM      '$@;)

BASIC_CFLAGS += -Wp,-MMD,$(@D)/$(@F).d -MT $(@D)/$(@F)

ifneq "$(MAKECMDGOALS)" "clean"
    DEP_FILES := $(addsuffix .d, $(OBJ))
    $(if $(DEP_FILES),$(eval -include $(DEP_FILES)))
endif
