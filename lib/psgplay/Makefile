# SPDX-License-Identifier: GPL-2.0

LIBPSGPLAY_STATIC := lib/psgplay/psgplay.a
LIBPSGPLAY_SHARED := lib/psgplay/psgplay.so

VER := lib/psgplay/version.c

PSGPLAY_SRC :=								\
	lib/psgplay/psgplay.c						\
	lib/psgplay/sndh.c						\
	$(VER)

.PHONY: $(shell script/version $(VER))
$(VER):
	@script/version $@

EXAMPLE_INFO_SRC := $(EXAMPLE_INFO).c

EXAMPLE_SRC := $(EXAMPLE_INFO_SRC)

EXAMPLE_LINK_OBJ +=							\
	lib/ice/ice.o							\
	system/unix/file.o						\
	system/unix/memory.o						\
	system/unix/print.o						\
	system/unix/string.o
