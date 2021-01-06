// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2021 Fredrik Noring
 */

#ifndef ATARI_TRACE_H
#define ATARI_TRACE_H

#define TRACE_DEVICE(dev)						\
	dev(all, ALL, 0)						\
	dev(cpu, CPU, 1)						\
	dev(reg, REG, 2)

enum trace_device {
#define TRACE_DEVICE_ENUM(symbol_, label_, id_)				\
	TRACE_DEVICE_##label_ = !id_ ? -1 : 1 << (id_ - 1),
TRACE_DEVICE(TRACE_DEVICE_ENUM)
};

#define TRACE_ENABLE(trace_mode_, label_)				\
	((trace_mode_)->m & TRACE_DEVICE_ ## label_)

struct trace_mode {
	uint32_t m;
};

#endif /* ATARI_TRACE_H */
