# SPDX-License-Identifier: GPL-2.0

ifeq (1,$(ALSA))
HAVE_CFLAGS += -DHAVE_ALSA
endif

ifeq (1,$(PORTAUDIO))
HAVE_CFLAGS += -DHAVE_PORTAUDIO
endif

AUDIO_SRC := $(addprefix lib/audio/,					\
	   alsa-writer.c						\
	   portaudio-writer.c						\
	   wave-reader.c						\
	   wave-writer.c						\
	   audio.c)
