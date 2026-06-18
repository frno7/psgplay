// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2021 Fredrik Noring
 */

#ifndef ATARI_TRACE_H
#define ATARI_TRACE_H

#include <stdio.h>
#include <stdint.h>

#define TRACE_DEVICE(dev)						\
	dev(all, ALL,  0)						\
	dev(cpu, CPU,  1)						\
	dev(irq, IRQ,  2)						\
	dev(mfp, MFP,  3)						\
	dev(psg, PSG,  4)						\
	dev(ram, RAM,  5)						\
	dev(reg, REG,  6)						\
	dev(rom, ROM,  7)						\
	dev(snd, SND,  8)						\
	dev(wch, WCH,  9)						\
	dev(zro, ZRO, 10)

enum trace_device {
	TRACE_DEVICE_NONE = 0,
#define TRACE_DEVICE_ENUM(symbol_, label_, id_)				\
	TRACE_DEVICE_##label_ = !id_ ? -1 : 1 << (id_ - 1),
TRACE_DEVICE(TRACE_DEVICE_ENUM)
};

#define TRACE_ENABLE(trace_mode_, label_)				\
	((trace_mode_) && ((trace_mode_)->m & TRACE_DEVICE_ ## label_))

struct trace_mode {
	uint32_t m;
	FILE *file;
	const char *output;
};

#endif /* ATARI_TRACE_H */
