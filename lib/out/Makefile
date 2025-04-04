# SPDX-License-Identifier: GPL-2.0

ifeq (1,$(ALSA))
HAVE_CFLAGS += -DHAVE_ALSA
endif

ifeq (1,$(PORTAUDIO))
HAVE_CFLAGS += -DHAVE_PORTAUDIO
endif

OUT_SRC := $(addprefix lib/out/,					\
	   alsa.c							\
	   portaudio.c				\
	   wave.c)
