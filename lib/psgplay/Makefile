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
