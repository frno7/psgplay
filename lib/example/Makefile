# SPDX-License-Identifier: GPL-2.0

EXAMPLE_INFO_SRC := $(EXAMPLE_INFO).c
EXAMPLE_PLAY_SRC := $(EXAMPLE_PLAY).c
EXAMPLE_INFO_OBJ := $(patsubst %.c, %.o, $(EXAMPLE_INFO_SRC))
EXAMPLE_PLAY_OBJ := $(patsubst %.c, %.o, $(EXAMPLE_PLAY_SRC))

EXAMPLE_SRC := $(EXAMPLE_INFO_SRC) $(EXAMPLE_PLAY_SRC)
EXAMPLE_OBJ := $(EXAMPLE_INFO_OBJ) $(EXAMPLE_PLAY_OBJ)

EXAMPLE_LINK_OBJ :=							\
	lib/ice/ice.o							\
	system/unix/file.o						\
	system/unix/memory.o						\
	system/unix/print.o						\
	system/unix/sndh.o						\
	system/unix/string.o
