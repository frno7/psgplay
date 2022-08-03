# SPDX-License-Identifier: GPL-2.0

PSGPLAY_CFLAGS = $(MOST_CFLAGS) $(HOST_CFLAGS)

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

PSGPLAY_object = $(addprefix system/unix/,$(subst /,-,$(1:%.c=%.o)))

define PSGPLAY_target
PSGPLAY_OBJ += $(call PSGPLAY_object,$(1))
$(call PSGPLAY_object,$(1)): $(1)
	$$(QUIET_CC)$$(HOST_CC) $$(PSGPLAY_CFLAGS) -c -o $$@ $$<
endef

$(foreach f,$(PSGPLAY_SRC),$(eval $(call PSGPLAY_target,$(f))))

ALL_OBJ += $(PSGPLAY_OBJ) system/unix/system-unix-disassemble.o

$(PSGPLAY): $(PSGPLAY_OBJ) $(LIBPSGPLAY_STATIC)
	$(QUIET_LINK)$(HOST_CC) $(PSGPLAY_CFLAGS) -o $@ $^ $(LIBS)

.PHONY: install-psgplay
install-psgplay: $(PSGPLAY)
	$(INSTALL) -d $(DESTDIR)$(bindir)
	$(INSTALL) $(PSGPLAY) $(DESTDIR)$(bindir)

OTHER_CLEAN += $(PSGPLAY)
