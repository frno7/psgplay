# SPDX-License-Identifier: GPL-2.0

PSGPLAY_LIB_TEST_CFLAGS = $(BASIC_HOST_CFLAGS) $(HOST_CFLAGS)

PSGPLAY_LIB_TEST_VERIFY := lib/example/example-info

PSGPLAY_LIB_TEST_SRC :=							\
	$(AUDIO_SRC)							\
	$(GRAPH_SRC)							\
	$(IN_SRC)							\
	lib/internal/print.c						\
	lib/internal/string.c						\
	lib/test/option.c						\
	lib/test/verify.c						\
	system/unix/file.c						\
	system/unix/memory.c						\
	system/unix/print.c						\
	system/unix/string.c

PSGPLAY_LIB_TEST_OBJ := $(PSGPLAY_LIB_TEST_SRC:%.c=%.o)

ALL_OBJ += $(PSGPLAY_LIB_TEST_OBJ)

$(PSGPLAY_LIB_TEST_OBJ): %.o: %.c
	$(QUIET_CC)$(HOST_CC) $(PSGPLAY_LIB_TEST_CFLAGS) -c -o $@ $<
