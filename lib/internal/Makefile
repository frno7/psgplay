# SPDX-License-Identifier: GPL-2.0

INTERNAL_SRC :=								\
	lib/internal/bit.c						\
	lib/internal/fifo.c						\
	lib/internal/print.c						\
	lib/internal/string.c

ALL_OBJ += lib/internal/sso.o

ifneq (clean,$(MAKECMDGOALS))

# Test whether the compiler supports scalar storage order.
HAVE_SSO := $(shell $(HOST_CC) $(HOST_CFLAGS) -c -o lib/internal/sso.o lib/internal/sso.c 2>&1 && echo 1)

ifeq (1,$(HAVE_SSO))
HAVE_CFLAGS += -DHAVE_SSO
else
$(warning WARNING: Disassembler disabled: The C compiler does not support __attribute__((__scalar_storage_order__("big-endian"))))
endif

endif
