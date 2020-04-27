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

M68K_SRC := $(M68K_GEN_C) lib/m68k/m68kcpu.c lib/m68k/m68kdasm.c	\
	lib/m68k/softfloat.c
