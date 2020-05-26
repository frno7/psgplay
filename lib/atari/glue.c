// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#include <stdio.h>

#include "internal/assert.h"

#include "atari/bus.h"
#include "atari/glue.h"
#include "atari/irq.h"
#include "atari/m68k.h"
#include "atari/mfp.h"
#include "atari/machine.h"

#include "m68k/m68k.h"

static struct device_cycle vbl_cycle;

static u32 irq_pending;

void glue_irq_set(int irq)
{
	const u32 p = irq_pending;

	BUG_ON(irq < 1 || 7 < irq);

	irq_pending |= 1 << irq;

	if (irq_pending > p)
		m68k_set_irq(irq);
}

void glue_irq_clr(int irq)
{
	int i;

	irq_pending &= ~(1u << irq);

	for (i = 7; i > 0; i--)
		if (irq_pending & (1u << i))
			break;

	m68k_set_irq(i);
}

static void request_vbl_event(struct device_cycle device_cycle)
{
	const u64 pal_vbl = 160256;	/* FIXME */

	vbl_cycle.c = device_cycle.c + (pal_vbl - (device_cycle.c % pal_vbl));

	request_device_event(&glue_device, vbl_cycle);
}

static void vbl_execute(struct device_cycle device_cycle)
{
	if (vbl_cycle.c)
		glue_irq_set(IRQ_VBL);
}

static int glue_hbl(void)
{
	glue_irq_clr(IRQ_VBL);

	return 26;	/* FIXME: HBL exception vector */
}

static int glue_vbl(void)
{
	glue_irq_clr(IRQ_VBL);

	return 28;	/* FIXME: VBL exception vector */
}

static int glue_mfp(void)
{
	glue_irq_clr(IRQ_MFP);

	return mfp_irq_vector();
}

static void glue_event(const struct device *device,
	struct device_cycle device_cycle)
{
	if (vbl_cycle.c <= device_cycle.c)
		vbl_execute(device_cycle);

	request_vbl_event(device_cycle);
}

static void glue_reset(const struct device *device)
{
	request_vbl_event(device_cycle(&glue_device));
}

int m68k_int_ack_callback(int level)
{
	switch(level)
	{
	case IRQ_HBL: return glue_hbl();
	case IRQ_VBL: return glue_vbl();
	case IRQ_MFP: return glue_mfp();
	default: return M68K_INT_ACK_SPURIOUS;
	}
}

const struct device glue_device = {
	.name = "glue",
	.frequency = 8000000,
	.reset = glue_reset,
	.event = glue_event,
};
