// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2026 Fredrik Noring
 */

#include <inttypes.h>
#include <stdio.h>
#include <string.h>

#include "internal/macro.h"
#include "internal/types.h"

#include "atari/bus.h"
#include "atari/exception-vector.h"
#include "atari/irq.h"
#include "atari/irq-trace.h"
#include "atari/machine.h"
#include "atari/mmu.h"
#include "atari/trace.h"

static uint32_t irq_vector_address(struct machine *machine, int vector)
{
	uint8_t b[4] = { };

	probe_copy_memory_16(machine, b, vector << 2, 2);

	return (b[0] << 24) |
	       (b[1] << 16) |
	       (b[2] <<  8) |
		b[3];
}

static const char *irq_vector_description(int vector)
{
	return exception_vector_description(vector << 2);
}

void irq_trace_ack(struct machine *machine, int level, int vector)
{
	if (!TRACE_ENABLE(machine->trace, IRQ))
		return;

	const uint32_t isp = m68k_get_reg(&machine->cpu.m68k, NULL, M68K_REG_ISP);

	if (machine->irq_trace.n >= ARRAY_SIZE(machine->irq_trace.stack)) {
		fprintf(machine->trace->file,
			"irq %8" PRIu64 "  %6x: ack OVERFLOW vector %d isp %x %s\n",
			machine_cycle(machine), irq_vector_address(machine, vector),
			vector, isp, irq_vector_description(vector));

		return;
	}

	fprintf(machine->trace->file,
		"irq %8" PRIu64 "  %6x: ack %d vector %d isp %x %s\n",
		machine_cycle(machine), irq_vector_address(machine, vector),
		machine->irq_trace.n, vector, isp,
		irq_vector_description(vector));

	machine->irq_trace.stack[machine->irq_trace.n] =
		(struct irq_trace_entry) {
			.level = level,
			.vector = vector,
			.isp = isp,
		};

	machine->irq_trace.n++;
}

void irq_trace_ret_probe(struct machine *machine)
{
	if (!TRACE_ENABLE(machine->trace, IRQ))
		return;

	const uint32_t isp = m68k_get_reg(&machine->cpu.m68k, NULL, M68K_REG_ISP);

	for (int k = machine->irq_trace.n; k > 0; k--) {
		const struct irq_trace_entry *e =
			&machine->irq_trace.stack[k - 1];

		if (e->isp > isp)
			break;

		machine->irq_trace.n--;

		fprintf(machine->trace->file,
			"irq %8" PRIu64 "  %6x: ret %d vector %d isp %x %s\n",
			machine_cycle(machine), machine->irq_trace.pc,
			machine->irq_trace.n, e->vector, isp,
			irq_vector_description(e->vector));
	}

	machine->irq_trace.pc = m68k_get_reg(&machine->cpu.m68k, NULL, M68K_REG_PC);
}
