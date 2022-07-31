# SPDX-License-Identifier: GPL-2.0
#
# Do "make help" for targets and options.

prefix = $(HOME)/.local/usr
datarootdir = $(prefix)/share
exec_prefix = $(prefix)
bindir = $(exec_prefix)/bin
mandir = $(datarootdir)/man
man1dir = $(mandir)/man1

INSTALL = install

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

EXAMPLE_INFO := lib/example/example-info
EXAMPLE_PLAY := lib/example/example-play

.PHONY: all
all: $(PSGPLAY) $(EXAMPLE_INFO) $(EXAMPLE_PLAY)

include lib/Makefile
include system/Makefile

ifneq "x$(CROSS_COMPILE)" "x"
all: $(PSGPLAY_TOS)
endif

PSGPLAY_SRC :=								\
	$(DISASSEMBLE_SRC) 						\
	$(ICE_SRC)							\
	$(OUT_SRC)							\
	$(SYSTEM_UNIX_SRC)						\
	$(TEXT_SRC)							\
	$(UNICODE_SRC)							\
	$(VT_SRC)
PSGPLAY_OBJ := $(patsubst %.c, %.o, $(PSGPLAY_SRC))

LIBPSGPLAY_HIDDEN_SRC :=						\
	$(ATARI_SRC)							\
	$(INTERNAL_SRC)							\
	$(M68K_SRC)
LIBPSGPLAY_HIDDEN_OBJ := $(patsubst %.c, %.o, $(LIBPSGPLAY_HIDDEN_SRC))

LIBPSGPLAY_PUBLIC_SRC := $(LIBPSGPLAY_SRC) $(VERSION_SRC)
LIBPSGPLAY_PUBLIC_OBJ := $(patsubst %.c, %.o, $(LIBPSGPLAY_PUBLIC_SRC))

LIBPSGPLAY_OBJ := $(LIBPSGPLAY_HIDDEN_OBJ) $(LIBPSGPLAY_PUBLIC_OBJ)

$(LIBPSGPLAY_STATIC): $(LIBPSGPLAY_OBJ)
	$(QUIET_AR)$(AR) rcs $@ $^

$(LIBPSGPLAY_SHARED): $(LIBPSGPLAY_OBJ)
	$(QUIET_CC)$(CC) $(SOFLAGS) -o $@ $^

$(PSGPLAY): $(PSGPLAY_OBJ) $(LIBPSGPLAY_STATIC)
	$(QUIET_LINK)$(CC) $(ALL_CFLAGS) -o $@ $^ $(LIBS)

$(EXAMPLE_INFO): $(EXAMPLE_INFO_OBJ) $(INTERNAL_OBJ)			\
	$(EXAMPLE_LINK_OBJ) $(LIBPSGPLAY_SHARED)
	$(QUIET_LINK)$(CC) $(ALL_CFLAGS) -o $@ $^

$(EXAMPLE_PLAY): $(EXAMPLE_PLAY_OBJ) $(INTERNAL_OBJ)			\
	$(EXAMPLE_LINK_OBJ) $(LIBPSGPLAY_SHARED)
	$(QUIET_LINK)$(CC) $(ALL_CFLAGS) -o $@ $^

$(LIBPSGPLAY_HIDDEN_OBJ): %.o : %.c
	$(QUIET_CC)$(CC) $(ALL_CFLAGS) -fvisibility=hidden -c -o $@ $<

$(LIBPSGPLAY_PUBLIC_OBJ) $(PSGPLAY_OBJ) $(EXAMPLE_OBJ): %.o : %.c
	$(QUIET_CC)$(CC) $(ALL_CFLAGS) -c -o $@ $<

.PHONY: install
install: install-bin install-man

.PHONY: install-bin
install-bin: $(PSGPLAY)
	$(INSTALL) -d $(DESTDIR)$(bindir)
	$(INSTALL) $(PSGPLAY) $(DESTDIR)$(bindir)

.PHONY: install-man
install-man:
	$(INSTALL) -d -m 755 $(DESTDIR)$(man1dir)
	$(INSTALL) -m 644 doc/psgplay.1 $(DESTDIR)$(man1dir)

.PHONY: test
test: $(M68KDT)
	$(QUIET_TEST)$(M68KDT) $(subst @,,$(V:1=-v))

.PHONY: version
version:
	@script/version

.PHONY: clean
clean:
	$(QUIET_RM)$(RM) -f */*.o* */*/*.o* include/tos/tos.h		\
		$(EXAMPLE_INFO) $(EXAMPLE_PLAY)				\
		$(PSGPLAY) PSGPLAY.* GPATH GRTAGS GTAGS 		\
		$(LIBPSGPLAY_STATIC) $(LIBPSGPLAY_SHARED)		\
		$(M68K_GEN_H) $(M68K_GEN_C) $(VERSION_SRC)		\
		$(M68KMAKE) $(M68KDG_GEN_H) $(M68KDG)

.PHONY: gtags
gtags:
	$(QUIET_GEN)gtags

.PHONY: help
help:
	@echo "Targets:"
	@echo "  all            - compile PSG play (default)"
	@echo "  install        - install PSG play"
	@echo "  test           - test components of PSG play"
	@echo "  PSGPLAY.TOS    - compile PSG play for Atari ST"
	@echo "  gtags          - make tags for the GNU Global source code tagging system"
	@echo "  version        - display PSG play version"
	@echo "  clean          - remove generated files"
	@echo
	@echo "Options:"
	@echo "  V              - set to 1 to compile verbosely"
	@echo "  S              - set to 1 for sanitation checks"
	@echo "  ALSA           - set to 1 to support ALSA for Linux"
	@echo "  CROSS_COMPILE  - set m68k cross compiler to use for Atari ST code"
	@echo
	@echo "Note that m68k-linux-* cross-compilers emit 68020 and will not work."
	@echo
	@echo "Example:"
	@echo "  make ALSA=1 CROSS_COMPILE=m68k-elf-"

V             = @
Q             = $(V:1=)
QUIET_AR      = $(Q:@=@echo    '  AR      '$@;)
QUIET_AS      = $(Q:@=@echo    '  AS      '$@;)
QUIET_CC      = $(Q:@=@echo    '  CC      '$@;)
QUIET_GEN     = $(Q:@=@echo    '  GEN     '$@;)
QUIET_LINK    = $(Q:@=@echo    '  LD      '$@;)
QUIET_RM      = $(Q:@=@echo    '  RM      '$@;)
QUIET_CHECK   = $(Q:@=@echo    '  CHECK   '$@;)
QUIET_TEST    = $(Q:@=@echo    '  TEST    '$@;)

BASIC_CFLAGS += -Wp,-MD,$(@D)/$(@F).d -MT $(@D)/$(@F)

ifneq "$(MAKECMDGOALS)" "clean"
    DEP_FILES := $(shell find . -name '*'.d -printf '%P\n' | sort)
    $(if $(DEP_FILES),$(eval include $(DEP_FILES)))
endif
