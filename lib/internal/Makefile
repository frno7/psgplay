# SPDX-License-Identifier: GPL-2.0

INTERNAL_SRC :=								\
	lib/internal/bit.c						\
	lib/internal/fifo.c						\
	lib/internal/print.c						\
	lib/internal/string.c
INTERNAL_OBJ = $(patsubst %.c, %.o, $(INTERNAL_SRC))

ifneq "$(MAKECMDGOALS)" "clean"

# Test whether the compiler supports scalar storage order.
HAVE_SSO := $(shell $(HOST_CC) $(HOST_CFLAGS) -c -o lib/internal/sso.o lib/internal/sso.c 2>&1 && echo 1)

ifeq "$(HAVE_SSO)" "1"
HAVE_CFLAGS += -DHAVE_SSO
else
$(warning WARNING: Disassembler disabled: The C compiler does not support __attribute__((__scalar_storage_order__("big-endian"))))
endif

endif
