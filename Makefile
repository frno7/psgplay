# SPDX-License-Identifier: GPL-2.0
#
# Do "make help" for targets and options.

prefix = $(HOME)/.local/usr
datarootdir = $(prefix)/share
exec_prefix = $(prefix)
bindir = $(exec_prefix)/bin
mandir = $(datarootdir)/man
man1dir = $(mandir)/man1
includedir = $(prefix)/include
libdir = $(exec_prefix)/lib
pkgdir = $(libdir)/pkgconfig

export prefix includedir libdir

BUILD_CC = $(CC)
HOST_AR = $(AR)
HOST_CC = $(CC)
# TARGET_CC = m68k-elf-gcc
# TARGET_LD = m68k-elf-ld

CFLAGS = -g -O2
BUILD_CFLAGS = $(CFLAGS)
HOST_CFLAGS = $(CFLAGS)
TARGET_CFLAGS = $(CFLAGS)

INSTALL = install

VERSION_MINOR = $(shell script/version | sed 's/-.*$$//')
VERSION_MAJOR = $(shell script/version | sed 's/\..*$$//')

export VERSION_MINOR VERSION_MAJOR

LIBS += -lm

SOFLAGS = -shared -Wl,-soname,libpsgplay.so.$(VERSION_MAJOR)

ifeq "$(S)" "1"
S_CFLAGS += -fsanitize=address -fsanitize=leak -fsanitize=undefined	\
	  -fsanitize-address-use-after-scope -fstack-check
endif

ifeq "$(ALSA)" "1"
HAVE_CFLAGS += -DHAVE_ALSA
LIBS += -lasound
endif

COMMON_CFLAGS = -Wall -fPIC -Iinclude -D_GNU_SOURCE
DEP_CFLAGS = $(COMMON_CFLAGS) $(BASIC_CFLAGS)
MOST_CFLAGS = $(HAVE_CFLAGS) $(S_CFLAGS) $(DEP_CFLAGS)

PSGPLAY := psgplay

EXAMPLE_INFO := lib/example/example-info
EXAMPLE_PLAY := lib/example/example-play

LIBPSGPLAY_PC := libpsgplay.pc

.PHONY: all
all: $(PSGPLAY) $(EXAMPLE_INFO) $(EXAMPLE_PLAY) $(LIBPSGPLAY_PC)

include lib/Makefile
include system/Makefile

ifneq "x$(TARGET_CC)" "x"
all: $(PSGPLAY_TOS)
endif

PSGPLAY_SRC :=								\
	$(DISASSEMBLE_SRC) 						\
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

LIBPSGPLAY_PUBLIC_SRC := $(LIBPSGPLAY_SRC) $(ICE_SRC) $(VERSION_SRC)
LIBPSGPLAY_PUBLIC_OBJ := $(patsubst %.c, %.o, $(LIBPSGPLAY_PUBLIC_SRC))

LIBPSGPLAY_OBJ := $(LIBPSGPLAY_HIDDEN_OBJ) $(LIBPSGPLAY_PUBLIC_OBJ)

$(LIBPSGPLAY_STATIC): $(LIBPSGPLAY_OBJ)
	$(QUIET_AR)$(HOST_AR) rcs $@ $^

$(LIBPSGPLAY_SHARED): $(LIBPSGPLAY_OBJ)
	$(QUIET_CC)$(HOST_CC) $(SOFLAGS) $(HOST_CFLAGS) -o $@ $^

$(LIBPSGPLAY_JAVASCRIPT) $(LIBPSGPLAY_WEBASSEMBLY): $(LIBPSGPLAY_OBJ)
	$(QUIET_CC)$(HOST_CC) $(WEBFLAGS) $(HOST_CFLAGS) -o $@ $^

.PHONY: web
web: $(LIBPSGPLAY_JAVASCRIPT)

$(PSGPLAY): $(PSGPLAY_OBJ) $(LIBPSGPLAY_STATIC)
	$(QUIET_LINK)$(HOST_CC) $(MOST_CFLAGS) $(HOST_CFLAGS) -o $@ $^ $(LIBS)

$(EXAMPLE_INFO): $(EXAMPLE_INFO_OBJ) $(INTERNAL_OBJ)			\
	$(EXAMPLE_LINK_OBJ) $(LIBPSGPLAY_SHARED)
	$(QUIET_LINK)$(HOST_CC) $(MOST_CFLAGS) $(HOST_CFLAGS) -o $@ $^

$(EXAMPLE_PLAY): $(EXAMPLE_PLAY_OBJ) $(INTERNAL_OBJ)			\
	$(EXAMPLE_LINK_OBJ) $(LIBPSGPLAY_SHARED)
	$(QUIET_LINK)$(HOST_CC) $(MOST_CFLAGS) $(HOST_CFLAGS) -o $@ $^

$(LIBPSGPLAY_HIDDEN_OBJ): %.o : %.c
	$(QUIET_CC)$(HOST_CC) $(MOST_CFLAGS) -fvisibility=hidden $(HOST_CFLAGS) -c -o $@ $<

$(LIBPSGPLAY_PUBLIC_OBJ) $(PSGPLAY_OBJ) $(EXAMPLE_OBJ): %.o : %.c
	$(QUIET_CC)$(HOST_CC) $(MOST_CFLAGS) $(HOST_CFLAGS) -c -o $@ $<

$(LIBPSGPLAY_PC):
	$(QUIET_GEN)script/pkg $@

.PHONY: install
install: install-bin install-man install-include install-lib install-pkg

.PHONY: install-bin
install-bin: $(PSGPLAY)
	$(INSTALL) -d $(DESTDIR)$(bindir)
	$(INSTALL) $(PSGPLAY) $(DESTDIR)$(bindir)

.PHONY: install-man
install-man:
	$(INSTALL) -d -m 755 $(DESTDIR)$(man1dir)
	$(INSTALL) -m 644 doc/psgplay.1 $(DESTDIR)$(man1dir)

.PHONY: install-include
install-include:
	$(INSTALL) -d -m 755 $(DESTDIR)$(includedir)/psgplay
	$(INSTALL) -m 644 include/ice/*.h $(DESTDIR)$(includedir)/psgplay
	$(INSTALL) -m 644 include/psgplay/*.h $(DESTDIR)$(includedir)/psgplay

.PHONY: install-lib
install-lib: install-lib-static install-lib-shared

.PHONY: install-lib-static
install-lib-static: $(LIBPSGPLAY_STATIC)
	$(INSTALL) -d -m 755 $(DESTDIR)$(libdir)
	$(INSTALL) $(LIBPSGPLAY_STATIC) $(DESTDIR)$(libdir)

.PHONY: install-lib-shared
install-lib-shared: $(LIBPSGPLAY_SHARED)
	$(INSTALL) -d -m 755 $(DESTDIR)$(libdir)
	$(INSTALL) $(LIBPSGPLAY_SHARED) $(DESTDIR)$(libdir)/libpsgplay.so.$(VERSION_MINOR)
	ln -s libpsgplay.so.$(VERSION_MINOR) $(DESTDIR)$(libdir)/libpsgplay.so.$(VERSION_MAJOR)
	ln -s libpsgplay.so.$(VERSION_MAJOR) $(DESTDIR)$(libdir)/libpsgplay.so

.PHONY: install-pkg
install-pkg: $(LIBPSGPLAY_PC)
	$(INSTALL) -d -m 755 $(DESTDIR)$(pkgdir)
	$(INSTALL) -m 644 $(LIBPSGPLAY_PC) $(DESTDIR)$(pkgdir)

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
		$(LIBPSGPLAY_JAVASCRIPT) $(LIBPSGPLAY_WEBASSEMBLY)	\
		$(M68K_GEN_H) $(M68K_GEN_C) $(VERSION_SRC)		\
		$(M68KMAKE) $(M68KDG_GEN_H) $(M68KDG)			\
		$(LIBPSGPLAY_PC)

.PHONY: gtags
gtags:
	$(QUIET_GEN)gtags

.PHONY: help
help:
	@echo "Targets:"
	@echo "  all            - compile PSG play (default)"
	@echo "  install        - install PSG play"
	@echo "  test           - test components of PSG play"
	@echo "  PSGPLAY.TOS    - compile PSG play for the Atari ST"
	@echo "  gtags          - make tags for the GNU Global source code tagging system"
	@echo "  version        - display PSG play version"
	@echo "  clean          - remove generated files"
	@echo
	@echo "Options:"
	@echo "  V              - set to 1 to compile verbosely"
	@echo "  S              - set to 1 for sanitation checks"
	@echo "  ALSA           - set to 1 to support ALSA for Linux"
	@echo
	@echo "  BUILD_CC       - set a C compiler to use for the build system"
	@echo "  HOST_AR        - set an archiver to use for the host system"
	@echo "  HOST_CC        - set a C compiler to use for the host system"
	@echo "  TARGET_CC      - set a m68k C compiler to use for Atari ST code"
	@echo "  TARGET_LD      - set a m68k linker to use for Atari ST code"
	@echo
	@echo "  BUILD_CFLAGS   - set C flags for the build system"
	@echo "  HOST_CFLAGS    - set C flags for the host system"
	@echo "  TARGET_CFLAGS  - set C flags for Atari ST code"
	@echo "  TARGET_LDFLAGS - set linker flags for Atari ST code"
	@echo
	@echo "Examples:"
	@echo
	@echo "Build an Atari ST program:"
	@echo "  make TARGET_CC=m68k-elf-gcc TARGET_LD=m68k-elf-ld PSGPLAY.TOS"
	@echo
	@echo "Build a statically linked program:"
	@echo '  make HOST_CFLAGS="-O2 -static" psgplay'
	@echo
	@echo "Build a cross-compiled program:"
	@echo "  make HOST_CC=mipsr5900el-unknown-linux-gnu-gcc psgplay"
	@echo
	@echo "Note that m68k-linux-* compilers emit 68020 and will not work."

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
