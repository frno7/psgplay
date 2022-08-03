# SPDX-License-Identifier: GPL-2.0

ifeq ($(ALSA),1)
HAVE_CFLAGS += -DHAVE_ALSA
endif

OUT_SRC := $(addprefix lib/out/,					\
	   alsa.c							\
	   wave.c)
