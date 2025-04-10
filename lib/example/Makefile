# SPDX-License-Identifier: GPL-2.0

EXAMPLE_CFLAGS = $(BASIC_HOST_CFLAGS) $(HOST_CFLAGS)

EXAMPLE_INFO := lib/example/example-info
EXAMPLE_PLAY := lib/example/example-play

EXAMPLE_INFO_SRC := $(EXAMPLE_INFO).c
EXAMPLE_PLAY_SRC := $(EXAMPLE_PLAY).c

EXAMPLE_INFO_OBJ := $(EXAMPLE_INFO_SRC:%.c=%.o)
EXAMPLE_PLAY_OBJ := $(EXAMPLE_PLAY_SRC:%.c=%.o)

EXAMPLE_LINK_SRC :=							\
	lib/ice/ice.c							\
	lib/internal/print.c						\
	lib/internal/string.c						\
	system/unix/file.c						\
	system/unix/memory.c						\
	system/unix/print.c						\
	system/unix/sndh.c						\
	system/unix/string.c

EXAMPLE_SRC := $(EXAMPLE_INFO_SRC) $(EXAMPLE_PLAY_SRC)
EXAMPLE_OBJ := $(EXAMPLE_INFO_OBJ) $(EXAMPLE_PLAY_OBJ)

EXAMPLE_object = $(addprefix lib/example/,$(subst /,-,$(1:%.c=%.o)))

define EXAMPLE_link_target
EXAMPLE_LINK_OBJ += $(call EXAMPLE_object,$(1))
$(call EXAMPLE_object,$(1)): $(1)
	$$(QUIET_CC)$$(HOST_CC) $$(EXAMPLE_CFLAGS) -c -o $$@ $$<
endef

$(foreach f,$(EXAMPLE_LINK_SRC),$(eval $(call EXAMPLE_link_target,$(f))))

ALL_OBJ += $(EXAMPLE_OBJ) $(EXAMPLE_LINK_OBJ)

$(EXAMPLE_OBJ): %.o: %.c
	$(QUIET_CC)$(HOST_CC) $(EXAMPLE_CFLAGS) -c -o $@ $<

$(EXAMPLE_INFO): $(EXAMPLE_INFO_OBJ) $(LIBPSGPLAY_SHARED)
$(EXAMPLE_PLAY): $(EXAMPLE_PLAY_OBJ) $(LIBPSGPLAY_SHARED)

.PHONY: example
example: $(EXAMPLE_INFO) $(EXAMPLE_PLAY)

$(EXAMPLE_INFO) $(EXAMPLE_PLAY): $(EXAMPLE_LINK_OBJ)
	$(QUIET_LINK)$(HOST_CC) $(EXAMPLE_CFLAGS) -o $@ $^

OTHER_CLEAN += $(EXAMPLE_INFO) $(EXAMPLE_PLAY)
