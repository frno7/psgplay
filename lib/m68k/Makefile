# SPDX-License-Identifier: GPL-2.0

M68KMAKE := lib/m68k/m68kmake

$(M68KMAKE).o: $(M68KMAKE).c
	$(QUIET_CC)$(BUILD_CC) $(MOST_CFLAGS) $(BUILD_CFLAGS) -c -o $@ $<
$(M68KMAKE): $(M68KMAKE).o
	$(QUIET_LINK)$(BUILD_CC) $(MOST_CFLAGS) $(BUILD_CFLAGS) -o $@ $^

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

M68KDG_LINK_SRC :=							\
	system/unix/file.c						\
	system/unix/memory.c						\
	system/unix/print.c						\
	system/unix/string.c						\
	lib/internal/bit.c						\
	lib/internal/fifo.c						\
	lib/internal/print.c						\
	lib/internal/string.c						\
	lib/version/version.c

define M68KDG_LINK_target
M68KDG_LINK_OBJ += lib/m68k/$(basename $(subst /,-,$(1))).o
lib/m68k/$(basename $(subst /,-,$(1))).o: $(1)
	$$(QUIET_CC)$$(BUILD_CC) $$(MOST_CFLAGS) $(BUILD_CFLAGS) -c -o $$@ $$<
endef

$(foreach f,$(M68KDG_LINK_SRC),$(eval $(call M68KDG_LINK_target,$(f))))

$(M68KDG).o: $(M68KDG).c
	$(QUIET_CC)$(BUILD_CC) $(MOST_CFLAGS) $(BUILD_CFLAGS) -c -o $@ $<

$(M68KDG): $(M68KDG).o $(M68KDG_LINK_OBJ)
	$(QUIET_LINK)$(BUILD_CC) $(MOST_CFLAGS) $(BUILD_CFLAGS) -o $@ $(M68KDG).o $(M68KDG_LINK_OBJ)

M68KDT := lib/m68k/m68kdt

M68KDA_SRC := lib/m68k/m68kds.c lib/m68k/m68kda.c
M68KDT_SRC := lib/m68k/m68kdt.c

M68KDA_OBJ := $(M68KDA_SRC:%.c=%.o)
M68KDT_OBJ := $(M68KDT_SRC:%.c=%.o)

ALL_OBJ += $(M68KDG).o $(M68KMAKE).o					\
	$(M68KDA_OBJ) $(M68KDT_OBJ) $(M68KDG_LINK_OBJ)

ifeq "$(HAVE_SSO)" "1"

M68K_SRC += $(M68KDA_SRC)
M68K_OBJ := $(M68K_SRC:%.c=%.o)

lib/m68k/m68kdt.c: $(M68KDG_GEN_H)

$(M68K_OBJ) $(M68KDT_OBJ): %.o : %.c
	$(QUIET_CC)$(HOST_CC) $(MOST_CFLAGS) $(HOST_CFLAGS) -c -o $@ $<

$(M68KDT): $(M68KDT_OBJ) $(M68KDG_LINK_OBJ) $(M68KDA_OBJ)
	$(QUIET_LINK)$(HOST_CC) $(MOST_CFLAGS) $(HOST_CFLAGS) -o $@	\
		$(M68KDT_OBJ) $(M68KDG_LINK_OBJ) $(M68KDA_OBJ)

.PHONY: test-m68kdt
test-m68kdt: $(M68KDT)
	$(QUIET_TEST)$(M68KDT) $(subst @,,$(V:1=-v))

endif
