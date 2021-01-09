# SPDX-License-Identifier: GPL-2.0

M68KMAKE := lib/m68k/m68kmake

$(M68KMAKE).o: $(M68KMAKE).c
	$(QUIET_CC)$(CC) $(ALL_CFLAGS) -c -o $@ $<
$(M68KMAKE): $(M68KMAKE).o
	$(QUIET_LINK)$(CC) $(ALL_CFLAGS) -o $@ $^

M68K_GEN_H := include/m68k/m68kops.h
M68K_GEN_C := lib/m68k/m68kops.c

lib/m68k/%ops.c include/m68k/%ops.h: lib/m68k/%_in.c $(M68KMAKE)
	$(Q:@=@echo    '  GEN     '$(M68K_GEN_H)			\
		$(M68K_GEN_C);)$(M68KMAKE) . $<

lib/m68k/m68kcpu.c: $(M68K_GEN_H)

M68K_SRC := $(M68K_GEN_C) lib/m68k/m68kcpu.c

M68KDA_SPEC := lib/m68k/m68kda.spec

M68KDG := lib/m68k/m68kdg

M68KDG_GEN_H := include/m68k/m68kdg.h

lib/m68k/m68kda.c: $(M68KDG_GEN_H)

$(M68KDG_GEN_H): $(M68KDG) $(M68KDA_SPEC)
	$(QUIET_GEN)$(M68KDG) -o $@ $(M68KDA_SPEC)

M68KDG_LINK_OBJS :=							\
	system/unix/file.o						\
	system/unix/memory.o						\
	system/unix/print.o						\
	system/unix/string.o						\
	lib/internal/bit.o						\
	lib/internal/fifo.o						\
	lib/internal/print.o						\
	lib/internal/string.o

$(M68KDG).o: $(M68KDG).c
	$(QUIET_CC)$(CC) $(ALL_CFLAGS) -c -o $@ $<

$(M68KDG): $(M68KDG).o $(M68KDG_LINK_OBJS)
	$(QUIET_LINK)$(CC) $(ALL_CFLAGS) -o $@ $(M68KDG).o $(M68KDG_LINK_OBJS)

ifeq "$(HAVE_SSO)" "1"

lib/m68k/m68kdt.c: $(M68KDG_GEN_H)

M68KDA_SRC := lib/m68k/m68kds.c lib/m68k/m68kda.c
M68KDT_SRC := lib/m68k/m68kds.c lib/m68k/m68kdt.c

M68K_SRC += $(M68KDA_SRC)

M68KDT_OBJ := $(patsubst %.c, %.o, $(M68KDT_SRC))

M68KDT_LINK_OBJ :=							\
	lib/psgplay/version.o						\
	system/unix/file.o						\
	system/unix/memory.o						\
	system/unix/print.o						\
	system/unix/string.o						\
	lib/internal/bit.o						\
	lib/internal/fifo.o						\
	lib/internal/print.o						\
	lib/internal/string.o

M68KDT := lib/m68k/m68kdt
$(M68KDT): $(M68KDT_OBJ) $(M68KDT_LINK_OBJ) lib/m68k/m68kda.o
	$(QUIET_LINK)$(CC) $(ALL_CFLAGS) -o $@ $^

endif
