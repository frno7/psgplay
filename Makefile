# SPDX-License-Identifier: GPL-2.0
#
# Do "make help" for targets and options.

CFLAGS += -g -O2 -Wall -fPIC -Iinclude -D_GNU_SOURCE

LIBS += -lm

SOFLAGS += -shared

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

EXAMPLE_INFO := lib/psgplay/example-info
EXAMPLE_PLAY := lib/psgplay/example-play

.PHONY: all
all: $(PSGPLAY) $(EXAMPLE_INFO) $(EXAMPLE_PLAY)

include lib/Makefile
include system/Makefile

LIBPSGPLAY_HIDDEN_SRC :=						\
	$(ATARI_SRC)							\
	$(INTERNAL_SRC)							\
	$(M68K_SRC)

LIBPSGPLAY_PUBLIC_SRC :=						\
	$(PSGPLAY_SRC)

PSGPLAY_SRC :=								\
	$(DISASSEMBLE_SRC) 						\
	$(ICE_SRC)							\
	$(OUT_SRC)							\
	$(SYSTEM_UNIX_SRC)						\
	$(TEXT_SRC)							\
	$(UNICODE_SRC)							\
	$(VT_SRC)

LIBPSGPLAY_HIDDEN_OBJ = $(patsubst %.c, %.o, $(LIBPSGPLAY_HIDDEN_SRC))
LIBPSGPLAY_PUBLIC_OBJ = $(patsubst %.c, %.o, $(LIBPSGPLAY_PUBLIC_SRC))
LIBPSGPLAY_OBJ = $(LIBPSGPLAY_HIDDEN_OBJ) $(LIBPSGPLAY_PUBLIC_OBJ)

INTERNAL_OBJ = $(patsubst %.c, %.o, $(INTERNAL_SRC))
PSGPLAY_OBJ = $(patsubst %.c, %.o, $(PSGPLAY_SRC))

EXAMPLE_INFO_OBJ = $(patsubst %.c, %.o, $(EXAMPLE_INFO_SRC))
EXAMPLE_PLAY_OBJ = $(patsubst %.c, %.o, $(EXAMPLE_PLAY_SRC))
EXAMPLE_OBJ = $(EXAMPLE_INFO_OBJ) $(EXAMPLE_PLAY_OBJ)

$(LIBPSGPLAY_STATIC): $(LIBPSGPLAY_OBJ)
	$(QUIET_AR)$(AR) rcs $@ $^

$(LIBPSGPLAY_SHARED): $(LIBPSGPLAY_OBJ)
	$(QUIET_CC)$(CC) $(SOFLAGS) -o $@ $^

$(PSGPLAY): $(PSGPLAY_OBJ) $(LIBPSGPLAY_STATIC)
	$(QUIET_LINK)$(CC) $(ALL_CFLAGS) -o $@ $^ $(LIBS)

$(EXAMPLE_INFO): $(EXAMPLE_INFO_OBJ) $(LIBPSGPLAY_SHARED)		\
	$(INTERNAL_OBJ) $(EXAMPLE_LINK_OBJ)
	$(QUIET_LINK)$(CC) $(ALL_CFLAGS) -o $@ $^

$(EXAMPLE_PLAY): $(EXAMPLE_PLAY_OBJ) $(LIBPSGPLAY_SHARED)		\
	$(INTERNAL_OBJ) $(EXAMPLE_LINK_OBJ)
	$(QUIET_LINK)$(CC) $(ALL_CFLAGS) -o $@ $^

$(LIBPSGPLAY_HIDDEN_OBJ): %.o : %.c
	$(QUIET_CC)$(CC) $(ALL_CFLAGS) -fvisibility=hidden -c -o $@ $<

$(LIBPSGPLAY_PUBLIC_OBJ): %.o : %.c
	$(QUIET_CC)$(CC) $(ALL_CFLAGS) -c -o $@ $<

$(PSGPLAY_OBJ) $(EXAMPLE_OBJ): %.o : %.c
	$(QUIET_CC)$(CC) $(ALL_CFLAGS) -c -o $@ $<

INSTALL = install

bindir  := $(prefix)/bin

install: all
	$(if $(prefix),,$(error "prefix" parameter must be set))
	$(INSTALL) -d $(DESTDIR)$(bindir)
	$(INSTALL) $(PSGPLAY) $(DESTDIR)$(bindir)

.PHONY: version
version:
	@script/version

.PHONY: clean
clean:
	$(QUIET_RM)$(RM) -f */*.o* */*/*.o* include/tos/tos.h		\
		$(EXAMPLE_INFO) $(EXAMPLE_PLAY)				\
		$(PSGPLAY) PSGPLAY.* GPATH GRTAGS GTAGS 		\
		$(LIBPSGPLAY_STATIC) $(LIBPSGPLAY_SHARED)		\
		$(M68K_GEN_H) $(M68K_GEN_C) $(VER) $(M68KMAKE)

.PHONY: gtags
gtags:
	@gtags

.PHONY: help
help:
	@echo "Targets:"
	@echo "  all            - compile PSG play (default)"
	@echo "  install        - install PSG play"
	@echo "  PSGPLAY.TOS    - compile PSG play for Atari ST"
	@echo "  version        - display PSG play version"
	@echo "  clean          - remove generated files"
	@echo
	@echo "Options:"
	@echo "  V              - set to 1 to compile verbosely"
	@echo "  S              - set to 1 for sanitation checks"
	@echo "  ALSA           - set to 1 to support ALSA for Linux"
	@echo "  CROSS_COMPILE  - set m68k cross compiler to use for Atari ST code"
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
QUIET_RM      = $(Q:@=@echo    '  RM      '$@;)
QUIET_TEST    = $(Q:@=@echo    '  TEST    '$@;)

BASIC_CFLAGS += -Wp,-MD,$(@D)/$(@F).d -MT $(@D)/$(@F)

ifneq "$(MAKECMDGOALS)" "clean"
    DEP_FILES := $(shell find . -name '*'.d -printf '%P\n' | sort)
    $(if $(DEP_FILES),$(eval include $(DEP_FILES)))
endif
