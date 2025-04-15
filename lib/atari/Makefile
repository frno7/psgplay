# SPDX-License-Identifier: GPL-2.0

lib/atari/ram.c lib/atari/rom.c: $(LIBPSGPLAY_TOS_HEADER)

ATARI_SRC := $(addprefix lib/atari/,					\
	bus.c								\
	cpu.c								\
	device.c							\
	exception-vector.c						\
	fdc.c								\
	glue.c								\
	machine.c							\
	mfp.c								\
	mixer.c								\
	mmu.c								\
	mmu-trace.c							\
	psg.c								\
	ram.c								\
	rom.c								\
	shifter.c							\
	sound.c								\
	system-variable.c)						\
	$(CF2149_SRC)
