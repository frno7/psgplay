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
pkgconfigdir = $(libdir)/pkgconfig

export prefix includedir libdir

BUILD_SYSTEM := $(shell uname -s)

LD = $(CC)

ifdef BUILD_COMPILE
BUILD_CC = $(BUILD_COMPILE)gcc
else
BUILD_CC = $(CC)
endif

ifdef HOST_COMPILE
HOST_CC = $(HOST_COMPILE)gcc
HOST_LD = $(HOST_COMPILE)gcc
HOST_AR = $(HOST_COMPILE)ar
else
HOST_CC = $(CC)
HOST_LD = $(LD)
ifeq (Darwin,$(BUILD_SYSTEM))
HOST_AR = libtool
else
HOST_AR = $(AR)
endif
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

ifeq (Darwin,$(BUILD_SYSTEM))
HOST_ARFLAGS = -static -o
SHLIB_EXT = dylib
else
HOST_ARFLAGS = rcs
SHLIB_EXT = so
endif

INSTALL = install

ifeq (1,$(S))
S_CFLAGS = -fsanitize=address -fsanitize=leak -fsanitize=undefined	\
	  -fsanitize-address-use-after-scope
endif

# Suppress false array-bounds warning "array subscript 0 is outside
# array bounds" and "source object is likely at address zero" for
# the io{rd,wr}{8,16,32} family of functions. Confer
# <https://gcc.gnu.org/bugzilla/show_bug.cgi?id=111035>.
TARGET_CFLAGS_FIXES = --param=min-pagesize=0

DEP_CFLAGS = -Wp,-MD,$(@D)/$(@F).d -MT $(@D)/$(@F)
BASIC_CFLAGS = -O2 -Wall -D_GNU_SOURCE $(HAVE_CFLAGS) $(DEP_CFLAGS)
BASIC_BUILD_CFLAGS = -Iinclude $(S_CFLAGS) $(BASIC_CFLAGS)
BASIC_HOST_CFLAGS = -Iinclude $(S_CFLAGS) $(BASIC_CFLAGS)
BASIC_TARGET_CFLAGS = -Iinclude $(TARGET_CFLAGS_FIXES) $(BASIC_CFLAGS)

.PHONY: all
all:

include doc/Makefile
include lib/Makefile
include system/Makefile
include test/Makefile

ALL_DEP = $(sort $(ALL_OBJ:%=%.d))

all: $(PSGPLAY)
all: $(LIBPSGPLAY_STATIC) $(LIBPSGPLAY_SHARED) $(LIBPSGPLAY_PC)
all: $(EXAMPLE_INFO) $(EXAMPLE_PLAY)

ifdef TARGET_CC
all: $(PSGPLAY_TOS)
endif

.PHONY: install
install: install-psgplay install-man install-lib

.PHONY: test
test: test-m68kdt
ifdef TARGET_CC
test: test-svg
endif

.PHONY: version
version:
	@script/version

.PHONY: gtags
gtags:
	$(QUIET_GEN)gtags
OTHER_CLEAN += GPATH GRTAGS GTAGS

.PHONY: clean
clean:
	$(QUIET_RM)$(RM) $(ALL_OBJ) $(ALL_DEP) $(OTHER_CLEAN)

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
QUIET_VERIFY  = $(Q:@=@echo    '  VERIFY  '$@;)

$(eval -include $(ALL_DEP))
