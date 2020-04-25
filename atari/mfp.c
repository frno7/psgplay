// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#include <stdio.h>
#include <string.h>

#include "atari/bus.h"
#include "atari/device.h"
#include "atari/irq.h"
#include "atari/glue.h"
#include "atari/machine.h"
#include "atari/mfp.h"

#include "psgplay/build-bug.h"

#define MFP_FREQUENCY	4000000

union mfp {
	struct {
#define MFP_REG_STRUCT(register_, symbol_, label_, description_)	\
	struct mfp_##symbol_ symbol_;
MFP_REGISTERS(MFP_REG_STRUCT)
	};
	u8 reg[24];
};

static union mfp mfp;

struct timer_cycle {
	u64 c;
};

struct timer {
	const char *name;
	u8 channel;
	u8 *data;
	u8 *ctrl;
	u8 ctrl_shift;

	struct timer_cycle timeout;
};

#define DEFINE_TIMER(symbol_, name_, channel_, dr_, cr_, cr_shift_)	\
static struct timer timer_##symbol_ = {					\
	.name = #name_,							\
	.channel = channel_,						\
	.data = &mfp.reg[MFP_REG_##dr_],				\
	.ctrl = &mfp.reg[MFP_REG_##cr_],				\
	.ctrl_shift = cr_shift_,					\
}

DEFINE_TIMER(a, A, 13, TADR, TACR,  0);
DEFINE_TIMER(b, B,  8, TBDR, TBCR,  0);
DEFINE_TIMER(c, C,  5, TCDR, TCDCR, 4);
DEFINE_TIMER(d, D,  4, TDDR, TCDCR, 0);

static char *mfp_register_name(u32 reg)
{
	switch (reg) {
#define MFP_REG_NAME(register_, symbol_, label_, description_)		\
	case register_: return #symbol_;
MFP_REGISTERS(MFP_REG_NAME)
	default:
		return "";
	}
}

static struct timer_cycle timer_from_mfp_cycle(
	const struct device_cycle mfp_cycle)
{
	return (struct timer_cycle) {
		.c = cycle_transform(
			MFP_TIMER_FREQUENCY, MFP_FREQUENCY, mfp_cycle.c)
	};
}

static struct device_cycle mfp_from_timer_cycle_align(
	const struct timer_cycle timer_cycle)
{
	return (struct device_cycle) {
		.c = cycle_transform_align(
			MFP_FREQUENCY, MFP_TIMER_FREQUENCY, timer_cycle.c)
	};
}

#define DEFINE_MFP_IR(symbol_, reg_)					\
static u16 mfp_##symbol_(void)						\
{									\
	return (mfp.reg[MFP_REG_##reg_##A] << 8) |			\
		mfp.reg[MFP_REG_##reg_##B];				\
}

DEFINE_MFP_IR(ier, IER);	/* 16-bit interrupt enable register */
DEFINE_MFP_IR(ipr, IPR);	/* 16-bit interrupt pending register */
DEFINE_MFP_IR(isr, ISR);	/* 16-bit interrupt service register */
DEFINE_MFP_IR(imr, IMR);	/* 16-bit interrupt mask register */

static u32 mfp_ctrl_prescale(const enum mfp_ctrl ctrl)
{
	switch (ctrl) {
#define MFP_CTRL_DIV_PRESCALE(div)					\
	case mfp_ctrl_div_##div: return div;
MFP_CTRL_DIV(MFP_CTRL_DIV_PRESCALE)
	default:
		BUG();
	}
}

static void mfp_wr_interrupt_pending(int i, bool pending)
{
	u8 *ipr = &mfp.reg[i < 8 ? MFP_REG_IPRB : MFP_REG_IPRA];
	u8 m = 1 << (i & 7);

	if (pending)
		*ipr |= m;
	else
		*ipr &= ~m;
}

static void mfp_wr_interrupt_service(int i, bool service)
{
	u8 *isr = &mfp.reg[i < 8 ? MFP_REG_ISRB : MFP_REG_ISRA];
	u8 m = 1 << (i & 7);

	if (service)
		*isr |= m;
	else
		*isr &= ~m;
}

static bool timer_rd_interrupt_enable(const struct timer *timer)
{
	return (mfp_ier() & (1 << timer->channel)) != 0;
}

static void timer_wr_interrupt_pending(const struct timer *timer, bool pending)
{
	mfp_wr_interrupt_pending(timer->channel, pending);
}

static u32 timer_period(const struct timer *timer)
{
	return *timer->data ? *timer->data : 256;
}

static u32 timer_ctrl(const struct timer *timer)
{
	return (*timer->ctrl >> timer->ctrl_shift) & 7;
}

static u32 timer_prescale(const struct timer *timer)
{
	return mfp_ctrl_prescale(timer_ctrl(timer));
}

static u8 timer_counter(struct timer *timer,
	const struct timer_cycle timer_cycle)
{
	if (timer_ctrl(timer) == mfp_ctrl_stop)
		return *timer->data;

	const u32 period = timer_period(timer);
	const u32 prescale = timer_prescale(timer);

	BUG_ON(!timer->timeout.c);

	if (timer_cycle.c < timer->timeout.c) {
		const u64 remaining = timer->timeout.c - timer_cycle.c;

		return (remaining + prescale - 1) / prescale;
	}

	const u64 elapsed = (timer_cycle.c - timer->timeout.c) / prescale;

	return period - (elapsed % period);
}

static void timer_delay_event(const struct device *device,
	struct timer *timer, const struct timer_cycle timer_cycle)
{
	if (timer_ctrl(timer) == mfp_ctrl_stop) {
		/* FIXME: Update counter with elapsed time */

		timer->timeout.c = 0;
		return;
	}

	/*
	 * A disabled channel is completely inactive; interrupts
	 * received on the channel are ignored by the MFP.
	 */
	if (!timer_rd_interrupt_enable(timer))
		return;

	if (timer->timeout.c && timer_cycle.c < timer->timeout.c)
		goto request_event;

	const u32 period = timer_period(timer);
	const u32 prescale = timer_prescale(timer);
	const u64 elapsed = timer->timeout.c ?
		timer_cycle.c - timer->timeout.c : 0;

	timer->timeout.c = timer_cycle.c +
		period * prescale - (elapsed % (period * prescale));

	/*
	 * When an interrupt is received on an enabled channel, the
	 * corresponding interrupt pending bit is set in IPRA or IPRB.
	 *
	 * In a vectored interrupt scheme, this bit will be cleared
	 * when the processor acknowledges the interrupting channel
	 * and the MFP responds with a vector number.
	 *
	 * In a polled interrupt scheme, the IPRs must be read to
	 * determine the interrupting channel, and then the interrupt
	 * pending bit is cleared by the interrupt handling routine
	 * without performing an interrupt acknowledge sequence.
	 */
	timer_wr_interrupt_pending(timer, true);

request_event:
	request_device_event(device, mfp_from_timer_cycle_align(timer->timeout));
}

static void mfp_event(const struct device *device,
	const struct device_cycle mfp_cycle)
{
	const struct timer_cycle timer_cycle = timer_from_mfp_cycle(mfp_cycle);

	if (mfp.tacr.pulse_width_mode)
		WARN_ONCE("mfp.tacr.pulse_width_mode not implemented\n");
	else
		timer_delay_event(device, &timer_a, timer_cycle);
	if (mfp.tbcr.pulse_width_mode)
		WARN_ONCE("mfp.tbcr.pulse_width_mode not implemented\n");
	else
		timer_delay_event(device, &timer_b, timer_cycle);

	timer_delay_event(device, &timer_c, timer_cycle);
	timer_delay_event(device, &timer_d, timer_cycle);

	/*
	 * Interrupts are masked for a channel by clearing the
	 * appropriate bit in IMRA or IMRB. Even though an enabled
	 * channel is masked, the channel will recognise subsequent
	 * interrupts and set its interrupt pending bit. However, the
	 * channel is prevented from requesting interrupt service
	 * (!IRQ to the processor) as long as the mask bit for that
	 * channel is cleared.
	 *
	 * If a channel is requesting interrupt service at the time
	 * that its corresponding bit in IMRA or IMRB is cleared, the
	 * request will cease, and !IRQ will be negated unless another
	 * channel is requesting interrupt service. Later, when the
	 * mask bit is set, any pending interrupt on the channel will
	 * be processed according to the channel's assigned priority.
	 * IMRA and IMRB may be read at any time.
	 */
	if ((mfp_ipr() & mfp_imr()) > mfp_isr())
		glue_irq_set(IRQ_MFP);
	else
		glue_irq_clr(IRQ_MFP);
}

u32 mfp_irq_vector(void)
{
	const u16 intr = mfp_ipr() & mfp_imr();
	const u16 isr = mfp_isr();

	for (int i = 15, m = 0x8000; m > isr; i--, m >>= 1) {
		if (!(intr & m))
			continue;

		/*
		 * In-service registers ISRA and ISRB allow interrupts to
		 * be nested. A bit is set whenever an interrupt vector
		 * is passed for an interrupt channel. The bit is cleared
		 * whenever the processor writes a zero to the bit.
		 *
		 * In an M68000 vectored interrupt system, the MFP is
		 * assigned to one of seven possible interrupt levels.
		 * When an interrupt is received from the MFP, an
		 * interrupt acknowledge for that level is initiated.
		 * Once an interrupt is recognised at a particular level,
		 * interrupts at the same level or below are masked by
		 * the processor.
		 *
		 * As long as the processor's interrupt mask is unchanged,
		 * the M68000 interrupt structure prohibits nesting the
		 * interrupts at the same interrupt level. However,
		 * additional interrupt requests from the MFP can be
		 * recognised before a previous channel's interrupt
		 * service routine is finished by lowering the processor's
		 * interrupt mask to the next lower interrupt level within
		 * the interrupt handler.
		 */
		mfp_wr_interrupt_pending(i, false);
		if (mfp.vr.sei)
			mfp_wr_interrupt_service(i, true);

		return (mfp.vr.base << 4) + i;
	}

	WARN_ONCE("M68K_INT_ACK_SPURIOUS");

	return M68K_INT_ACK_SPURIOUS;
}

static u8 mfp_rd_u8(const struct device *device, u32 dev_address)
{
	const struct device_cycle mfp_cycle = device_cycle(device);
	const struct timer_cycle timer_cycle = timer_from_mfp_cycle(mfp_cycle);
	const u32 reg = dev_address >> 1;

	if ((dev_address & 1) == 0 || ARRAY_SIZE(mfp.reg) <= reg)
		return 0;

	switch (reg) {
	case MFP_REG_TADR: return timer_counter(&timer_a, timer_cycle);
	case MFP_REG_TBDR: return timer_counter(&timer_b, timer_cycle);
	case MFP_REG_TCDR: return timer_counter(&timer_c, timer_cycle);
	case MFP_REG_TDDR: return timer_counter(&timer_d, timer_cycle);
	default:	   return mfp.reg[reg];
	}
}

static u16 mfp_rd_u16(const struct device *device, u32 dev_address)
{
	return mfp_rd_u8(device, dev_address + 1);
}

static void mfp_hardwire(void)
{
	mfp.vr.unused = 0;
	mfp.tacr.unused = 0;
	mfp.tbcr.unused = 0;
	mfp.tcdcr.tc_unused = 0;
	mfp.tcdcr.td_unused = 0;
	mfp.ucr.unused = 0;
}

static void mfp_wr_u8(const struct device *device,
	u32 dev_address, u8 data)
{
	const u32 reg = dev_address >> 1;
	const bool sei = mfp.vr.sei;

	if ((dev_address & 1) == 0 || ARRAY_SIZE(mfp.reg) <= reg)
		return;

	mfp.reg[reg] = data;
	mfp_hardwire();

	switch (reg) {
	case MFP_REG_IERA:
	case MFP_REG_IERB:
		/*
		 * Writing a zero to a bit of IERA or IERB causes the
		 * corresponding bit of the IPR to be cleared, which
		 * terminates all interrupt service requests for the
		 * channel and also negates !IRQ unless interrupts are
		 * pending from other sources.
		 *
		 * Disabling a channel, however, does not affect the
		 * corresponding bit in ISRA or ISRB. Therefore, since
		 * the MFP for the Atari ST is in the software end-of-
		 * interrupt mode and an interrupt is in service when
		 * a channel is disabled, the in-service bit of that
		 * channel will remain set until cleared by software.
		 */
		mfp.reg[MFP_REG_IPRA] &= mfp.reg[MFP_REG_IERA];
		mfp.reg[MFP_REG_IPRB] &= mfp.reg[MFP_REG_IERB];
		break;

	case MFP_REG_VR:
		/*
		 * The interrupt in-service registers ISRA and ISRB
		 * are cleared if the S-bit of the vector register is
		 * cleared.
		 */
		if (sei && !mfp.vr.sei)
			mfp.reg[MFP_REG_ISRA] = mfp.reg[MFP_REG_ISRB] = 0;
		break;
	}

	mfp_event(device, device_cycle(device));
}

static void mfp_wr_u16(const struct device *device, u32 dev_address, u16 data)
{
	mfp_wr_u8(device, dev_address + 1, data & 0xff);
}

static size_t mfp_id_u8(const struct device *device,
	u32 dev_address, char *buf, size_t size)
{
	const u32 reg = dev_address >> 1;

	if ((dev_address & 1) == 0 || ARRAY_SIZE(mfp.reg) <= reg)
		snprintf(buf, size, "%2u", dev_address);
	else
		snprintf(buf, size, "%s", mfp_register_name(reg));

	return strlen(buf);
}

static size_t mfp_id_u16(const struct device *device,
	u32 dev_address, char *buf, size_t size)
{
	return mfp_id_u8(device, dev_address + 1, buf, size);
}

static void mfp_reset(const struct device *device)
{
	BUILD_BUG_ON(sizeof(mfp) != 24);

	memset(&mfp, 0, sizeof(mfp));
	mfp_hardwire();

	timer_a.timeout.c = 0;
	timer_b.timeout.c = 0;
	timer_c.timeout.c = 0;
	timer_d.timeout.c = 0;
}

const struct device mfp_device = {
	.name = "mfp",
	.frequency = MFP_FREQUENCY,
	.bus = {
		.address = 0xfffa00,
		.size = 64,
	},
	.reset = mfp_reset,
	.event = mfp_event,
	.rd_u8  = mfp_rd_u8,
	.rd_u16 = mfp_rd_u16,
	.wr_u8  = mfp_wr_u8,
	.wr_u16 = mfp_wr_u16,
	.id_u8  = mfp_id_u8,
	.id_u16 = mfp_id_u16,
};
