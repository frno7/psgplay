# SPDX-License-Identifier: GPL-2.0
#
# See the file INSTALL for installation instructions.

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

ifdef BUILD_COMPILE
BUILD_CC = $(BUILD_COMPILE)gcc
else
BUILD_CC = $(CC)
endif

ifdef HOST_COMPILE
HOST_CC = $(HOST_COMPILE)gcc
HOST_AR = $(HOST_COMPILE)ar
else
HOST_CC = $(CC)
HOST_AR = $(AR)
endif

ifdef TARGET_COMPILE
TARGET_CC = $(TARGET_COMPILE)gcc
TARGET_LD = $(TARGET_COMPILE)ld
TARGET_AR = $(TARGET_COMPILE)ar
endif

CFLAGS = -g
BUILD_CFLAGS = $(CFLAGS)
HOST_CFLAGS = $(CFLAGS)
TARGET_CFLAGS = $(CFLAGS)

INSTALL = install
XXD = xxd

export XXD

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

DEP_CFLAGS = -Wp,-MD,$(@D)/$(@F).d -MT $(@D)/$(@F)
COMMON_CFLAGS = -O2 -Wall -fPIC -Iinclude -D_GNU_SOURCE
SOME_CFLAGS = $(COMMON_CFLAGS) $(DEP_CFLAGS)
MOST_CFLAGS = $(HAVE_CFLAGS) $(S_CFLAGS) $(SOME_CFLAGS)

.PHONY: all
all:

include lib/Makefile
include system/Makefile

all: $(PSGPLAY)
all: $(LIBPSGPLAY_STATIC) $(LIBPSGPLAY_SHARED) $(LIBPSGPLAY_PC)
all: $(EXAMPLE_INFO) $(EXAMPLE_PLAY)

ifdef TARGET_CC
all: $(PSGPLAY_TOS)
endif

.PHONY: web
web: $(LIBPSGPLAY_JAVASCRIPT)

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
		$(PSGPLAY) $(PSGPLAY_TOS) GPATH GRTAGS GTAGS 		\
		$(LIBPSGPLAY_STATIC) $(LIBPSGPLAY_SHARED)		\
		$(LIBPSGPLAY_JAVASCRIPT) $(LIBPSGPLAY_WEBASSEMBLY)	\
		$(M68K_GEN_H) $(M68K_GEN_C) $(VERSION_SRC)		\
		$(M68KMAKE) $(M68KDG_GEN_H) $(M68KDG) $(M68KDT)		\
		$(LIBPSGPLAY_PC)

.PHONY: gtags
gtags:
	$(QUIET_GEN)gtags

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

ifneq "$(MAKECMDGOALS)" "clean"
    DEP_FILES := $(shell find . -name '*'.d -printf '%P\n' | sort)
    $(if $(DEP_FILES),$(eval include $(DEP_FILES)))
endif
