# SPDX-License-Identifier: GPL-2.0

SYSTEM_UNIX_SRC =							\
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
	system/unix/string.c						\
	system/unix/text-mode.c						\
	system/unix/tty.c

ifeq "$(HAVE_SSO)" "1"
SYSTEM_UNIX_SRC += system/unix/disassemble.c
endif
