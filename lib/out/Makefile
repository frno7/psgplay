# SPDX-License-Identifier: GPL-2.0

ifeq (1,$(ALSA))
HAVE_CFLAGS += -DHAVE_ALSA
endif

OUT_SRC := $(addprefix lib/out/,					\
	   alsa.c							\
	   wave.c)
