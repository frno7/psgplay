# SPDX-License-Identifier: GPL-2.0

include lib/version/Makefile
include lib/tos/Makefile
include lib/ice/Makefile
include lib/internal/Makefile
include lib/m68k/Makefile
include lib/atari/Makefile

include lib/psgplay/Makefile

include lib/example/Makefile

ifdef TARGET_CC
TOSLIBC_CFLAGS = $(BASIC_CFLAGS)
include lib/toslibc/lib/Makefile
endif

include lib/out/Makefile
include lib/vt/Makefile
include lib/unicode/Makefile
include lib/text/Makefile
