# SPDX-License-Identifier: GPL-2.0

PSGPLAY := psgplay

SYSTEM_UNIX_SRC :=							\
	system/unix/clock.c						\
	system/unix/command-mode.c					\
	system/unix/diagnostic.c					\
	system/unix/file.c						\
	system/unix/info.c						\
	system/unix/memory.c						\
	system/unix/option.c						\
	system/unix/poll-fifo.c						\
	system/unix/print.c						\
	system/unix/psgplay.c						\
	system/unix/remake.c						\
	system/unix/sndh.c						\
	system/unix/string.c						\
	system/unix/text-mode.c						\
	system/unix/tty.c

ifeq "$(HAVE_SSO)" "1"
SYSTEM_UNIX_SRC += system/unix/disassemble.c
endif

PSGPLAY_SRC :=								\
	$(DISASSEMBLE_SRC) 						\
	$(OUT_SRC)							\
	$(SYSTEM_UNIX_SRC)						\
	$(TEXT_SRC)							\
	$(UNICODE_SRC)							\
	$(VT_SRC)

PSGPLAY_OBJ := $(patsubst %.c, %.o, $(PSGPLAY_SRC))

$(PSGPLAY_OBJ): %.o : %.c
	$(QUIET_CC)$(HOST_CC) $(MOST_CFLAGS) $(HOST_CFLAGS) -c -o $@ $<

$(PSGPLAY): $(PSGPLAY_OBJ) $(LIBPSGPLAY_STATIC)
	$(QUIET_LINK)$(HOST_CC) $(MOST_CFLAGS) $(HOST_CFLAGS) -o $@ $^ $(LIBS)
