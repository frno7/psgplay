# SPDX-License-Identifier: GPL-2.0

VER := lib/psgplay/version.c

PSGPLAY_SRC :=								\
	lib/psgplay/ice.c						\
	lib/psgplay/psgplay.c						\
	lib/psgplay/sndh.c						\
	$(VER)

.PHONY: $(shell script/version $(VER))
$(VER):
	@script/version $@
