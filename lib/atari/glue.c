// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#include <stdio.h>

#include "toslibc/asm/machine.h"

#include "atari/bus.h"
#include "atari/glue.h"
#include "atari/irq.h"
#include "atari/m68k.h"
#include "atari/mfp.h"
#include "atari/machine.h"

#include "m68k/m68k.h"
#include "m68k/m68kcpu.h"

void glue_irq_set(struct machine *machine, int irq)
{
	const u32 p = machine->glue.irq_pending;

#if 0  /* FIXME: Dependency on pr_bug */
	BUG_ON(irq < 1 || 7 < irq);
#endif

	machine->glue.irq_pending |= 1 << irq;

	if (machine->glue.irq_pending > p)
		m68k_set_irq(&machine->cpu.m68k, irq);
}

void glue_irq_clr(struct machine *machine, int irq)
{
	int i;

	machine->glue.irq_pending &= ~(1u << irq);

	for (i = 7; i > 0; i--)
		if (machine->glue.irq_pending & (1u << i))
			break;

	m68k_set_irq(&machine->cpu.m68k, i);
}

static void request_vbl_event(struct machine *machine,
	struct device_cycle device_cycle)
{
	const u64 pal_vbl = ATARI_STE_CYCLES_PER_VBL_PAL;

	machine->glue.vbl_cycle.c = device_cycle.c + (pal_vbl - (device_cycle.c % pal_vbl));

	request_device_event(machine, &glue_device, machine->glue.vbl_cycle);
}

static void vbl_execute(struct machine *machine,
	struct device_cycle device_cycle)
{
	if (machine->glue.vbl_cycle.c)
		glue_irq_set(machine, IRQ_VBL);
}

static int glue_hbl(struct machine *machine)
{
	glue_irq_clr(machine, IRQ_VBL);

	return 26;	/* FIXME: HBL exception vector */
}

static int glue_vbl(struct machine *machine)
{
	glue_irq_clr(machine, IRQ_VBL);

	return 28;	/* FIXME: VBL exception vector */
}

static int glue_mfp(struct machine *machine)
{
	glue_irq_clr(machine, IRQ_MFP);

	return mfp_irq_vector(machine);
}

static void glue_event(struct machine *machine, const struct device *device,
	struct device_cycle device_cycle)
{
	if (machine->glue.vbl_cycle.c <= device_cycle.c)
		vbl_execute(machine, device_cycle);

	request_vbl_event(machine, device_cycle);
}

static void glue_reset(struct machine *machine, const struct device *device)
{
	request_vbl_event(machine, device_cycle(machine, &glue_device));
}

int m68k_int_ack_callback(struct m68k_module *module, int level)
{
	struct machine *machine = machine_from_m68k_module(module);

	switch(level)
	{
	case IRQ_HBL: return glue_hbl(machine);
	case IRQ_VBL: return glue_vbl(machine);
	case IRQ_MFP: return glue_mfp(machine);
	default: return M68K_INT_ACK_SPURIOUS;
	}
}

const struct device glue_device = {
	.name = "glue",
	.clk = {
		.frequency = CPU_FREQUENCY,
		.divisor = 1
	},
	.reset = glue_reset,
	.event = glue_event,
};
