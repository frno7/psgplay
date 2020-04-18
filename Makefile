# SPDX-License-Identifier: GPL-2.0
#
# Define V=1 for more verbose compile.
# Define S=1 for sanitation checks.
#
# Define ALSA=1 for Advanced Linux Sound Architecture (ALSA) support
# Define TOS=1 to compile TOS stub

CFLAGS += -g -O2 -Wall -Iinclude -D_GNU_SOURCE

LIBS += -lm

ifeq "$(S)" "1"
CFLAGS += -fsanitize=address -fsanitize=leak -fsanitize=undefined	\
	  -fsanitize-address-use-after-scope -fstack-check
endif

ifeq "$(ALSA)" "1"
CFLAGS += -DHAVE_ALSA
LIBS += -lasound
endif

ALL_CFLAGS += $(CFLAGS) $(BASIC_CFLAGS)

.PHONY: all
all: tool

PSGPLAY := tool/psgplay

.PHONY: tool
tool: $(PSGPLAY)

M68KMAKE := m68k/m68kmake

$(M68KMAKE).o: $(M68KMAKE).c
	$(QUIET_CC)$(CC) $(ALL_CFLAGS) -c -o $@ $<
$(M68KMAKE): $(M68KMAKE).o
	$(QUIET_LINK)$(CC) $(ALL_CFLAGS) -o $@ $^

M68K_GEN_H := include/m68k/m68kops.h
M68K_GEN_C := m68k/m68kops.c

m68k/%ops.c include/m68k/%ops.h: m68k/%_in.c $(M68KMAKE)
	$(Q:@=@echo    '  GEN     '$(M68K_GEN_H)			\
		$(M68K_GEN_C);)$(M68KMAKE) . $<

m68k/m68kcpu.c: $(M68K_GEN_H)

M68K_C := $(M68K_GEN_C) m68k/m68kcpu.c m68k/m68kdasm.c m68k/softfloat.c

ifeq "$(TOS)" "1"
tos/tos.o: tos/tos.s
	$(QUIET_AS)m68k-as -o $@ $<
tos/tos: script/tos.ld script/tos
tos/tos: tos/tos.o
	$(QUIET_LINK)m68k-ld --orphan-handling=error			\
		--discard-all -nostdlib --no-relax			\
		--script=script/tos.ld -o $@ $<
	@chmod a-x $@
else
tos/tos:
	@touch $@
endif
include/tos/tos.h: tos/tos
	$(QUIET_GEN)script/tos $< $@
atari/rom.c: include/tos/tos.h

VER := tool/version.c
SRC := $(filter-out $(VER), $(wildcard tool/*.c))			\
	$(wildcard atari/*.c) $(M68K_C) $(VER)
OBJ = $(patsubst %.c, %.o, $(SRC))

$(PSGPLAY): $(OBJ)
	$(QUIET_LINK)$(CC) $(ALL_CFLAGS) -o $@ $^ $(LIBS)

$(OBJ): %.o : %.c
	$(QUIET_CC)$(CC) $(ALL_CFLAGS) -c -o $@ $<

.PHONY: $(shell script/version $(VER))
$(VER):
	@script/version $@

.PHONY: clean
clean:
	$(QUIET_RM)$(RM) -f */*.o */*.o.d include/tos/tos.h		\
		GPATH GRTAGS GTAGS 					\
		$(M68K_GEN_H) $(M68K_GEN_C) $(VER) $(PSGPLAY) $(M68KMAKE)

.PHONY: gtags
gtags:
	gtags

V             = @
Q             = $(V:1=)
QUIET_AR      = $(Q:@=@echo    '  AR      '$@;)
QUIET_AS      = $(Q:@=@echo    '  AS      '$@;)
QUIET_CC      = $(Q:@=@echo    '  CC      '$@;)
QUIET_GEN     = $(Q:@=@echo    '  GEN     '$@;)
QUIET_LINK    = $(Q:@=@echo    '  LD      '$@;)
QUIET_TEST    = $(Q:@=@echo    '  TEST    '$@;)
QUIET_RM      = $(Q:@=@echo    '  RM      '$@;)

BASIC_CFLAGS += -Wp,-MMD,$(@D)/$(@F).d -MT $(@D)/$(@F)

ifneq "$(MAKECMDGOALS)" "clean"
    DEP_FILES := $(addsuffix .d, $(OBJ))
    $(if $(DEP_FILES),$(eval -include $(DEP_FILES)))
endif
