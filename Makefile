# SPDX-License-Identifier: GPL-2.0
#
# Do "make help" for targets and options.

CFLAGS += -g -O2 -Wall -Iinclude -D_GNU_SOURCE

LIBS += -lm

ifeq "$(S)" "1"
S_CFLAGS += -fsanitize=address -fsanitize=leak -fsanitize=undefined	\
	  -fsanitize-address-use-after-scope -fstack-check
endif

ifeq "$(ALSA)" "1"
HAVE_CFLAGS += -DHAVE_ALSA
LIBS += -lasound
endif

DEP_CFLAGS += $(CFLAGS) $(BASIC_CFLAGS)
ALL_CFLAGS += $(DEP_CFLAGS) $(HAVE_CFLAGS) $(S_CFLAGS)

PSGPLAY := psgplay

.PHONY: all
all: $(PSGPLAY)

include lib/Makefile
include system/Makefile

SRC := $(ATARI_SRC) $(M68K_SRC) $(OUT_SRC) $(PSGPLAY_SRC) $(INTERNAL_SRC)\
	$(SYSTEM_UNIX_SRC) $(UNICODE_SRC)
OBJ = $(patsubst %.c, %.o, $(SRC))

$(PSGPLAY): $(OBJ)
	$(QUIET_LINK)$(CC) $(ALL_CFLAGS) -o $@ $^ $(LIBS)

$(OBJ): %.o : %.c
	$(QUIET_CC)$(CC) $(ALL_CFLAGS) -c -o $@ $<

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

BASIC_CFLAGS += -Wp,-MD,$(@D)/$(@F).d -MT $(@D)/$(@F)

ifneq "$(MAKECMDGOALS)" "clean"
    DEP_FILES := $(shell find . -name '*'.d -printf '%P\n' | sort)
    $(if $(DEP_FILES),$(eval include $(DEP_FILES)))
endif
