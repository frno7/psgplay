// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2026 Fredrik Noring
 */

#ifndef ATARI_IRQ_TRACE_H
#define ATARI_IRQ_TRACE_H

#include "internal/types.h"

#include "atari/device.h"

void irq_trace_ack(struct machine *machine, int level, int vector);

void irq_trace_ret_probe(struct machine *machine);

#endif /* ATARI_IRQ_TRACE_H */
