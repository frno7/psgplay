// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#include "tos/stdlib.h"

#include "psgplay/macro.h"
#include "psgplay/types.h"
#include "psgplay/sndh-timer.h"

#include "atari/mfp-map.h"

struct timer_prescale {
	enum mfp_ctrl ctrl;
	u8 count;
};

static struct record {
	size_t size;
	struct {
		u32 init;
		u32 exit;
		u32 play;
		u8 data[];
	} *sndh;
} record;

static struct {
	enum sndh_timer_type type;
	struct timer_prescale prescale;
} timer_state;

static void sndh_init(int track)
{
	__asm__ __volatile__ (
		"	movem.l	%%d0-%%a6,-(%%sp)\n"
		"	move.l	%1,%%d0\n"
		"	jsr	(%0)\n"
		"	movem.l	(%%sp)+,%%d0-%%a6\n"
		:
		: "a" (&record.sndh->init), "d" (track)
		: "memory");
}

static void sndh_play(void)
{
	__asm__ __volatile__ (
		/* Enable interrupts and other timers for effects. */
		"	move	#0x2200,%%sr\n"
		"	movem.l	%%d0-%%a6,-(%%sp)\n"
		"	jsr	(%0)\n"
		"	movem.l	(%%sp)+,%%d0-%%a6\n"
		:
		: "a" (&record.sndh->play)
		: "memory");
}

static bool timer_prescale(struct timer_prescale *prescale, int frequency)
{
	static const u8 prescale_div[] = {
#define MFP_CTRL_DIV_PRESCALE(div) div,
MFP_CTRL_DIV(MFP_CTRL_DIV_PRESCALE)
	};
	bool valid = false;
	int best = -1;

	/*
	 * FIXME: The somewhat contrived looping and limitations here are to
	 * avoid linking with __mulsi3, __divsi3, etc. for the time being.
	 */

	if (frequency < 1 || 320 < frequency) /* FIXME: 65536 / 200 = 327 */
		return false;

	for (size_t i = 0; i < ARRAY_SIZE(prescale_div); i++) {
		const u16 div = frequency * prescale_div[i];

		for (u16 count = 1; count <= 256; count++) {
			const int f = count * div;
			const int diff = abs(f - MFP_TIMER_FREQUENCY);

			if (best >= 0 && diff >= best)
				continue;

			best = diff;
			valid = true;
			*prescale = (struct timer_prescale) {
				.ctrl = 1 + i,
				.count = count & 0xff,
			};
		}
	}

	return valid;
}

void vbl_exception(void)
{
	if (timer_state.type == SNDH_TIMER_V)
		sndh_play();
}

static bool install_sndh_vbl(int frequency)
{
	timer_state.type = SNDH_TIMER_V;

	return true;	/* The VBL frequency is constant. */
}

#define DEFINE_TIMER_EXCEPTION(name_, type_, ab_, ctrl_)		\
	void timer_##name_##_exception(void)				\
	{								\
		if (timer_state.type == SNDH_TIMER_##type_)		\
			sndh_play();					\
									\
		/* Some SNDH files mess with the counters, restore. */	\
		mfp_map()->ctrl_ = timer_state.prescale.ctrl;		\
		mfp_map()->t##name_##dr.count = timer_state.prescale.count; \
									\
		/* Timer is now served. */				\
		mfp_map()->is##ab_.timer_##name_ = false;		\
	}								\
									\
	static bool install_sndh_timer_##name_(int frequency)		\
	{								\
		if (!timer_prescale(&timer_state.prescale, frequency))	\
			return false;					\
									\
		mfp_map()->ctrl_ = timer_state.prescale.ctrl;		\
		mfp_map()->t##name_##dr.count = timer_state.prescale.count; \
		mfp_map()->ie##ab_.timer_##name_ = true;		\
		mfp_map()->ip##ab_.timer_##name_ = false;		\
		mfp_map()->is##ab_.timer_##name_ = false;		\
		mfp_map()->im##ab_.timer_##name_ = true;		\
									\
		timer_state.type = SNDH_TIMER_##type_;			\
									\
		return true;						\
	}

DEFINE_TIMER_EXCEPTION(a, A, ra, tacr.ctrl)
DEFINE_TIMER_EXCEPTION(b, B, ra, tbcr.ctrl)
DEFINE_TIMER_EXCEPTION(c, C, rb, tcdcr.tc_ctrl)
DEFINE_TIMER_EXCEPTION(d, D, rb, tcdcr.td_ctrl)

static bool install_sndh_timer(const struct sndh_timer timer)
{
	bool (* const install_sndh_timer)(int frequency) =
		(timer.type == SNDH_TIMER_A ? install_sndh_timer_a :
		 timer.type == SNDH_TIMER_B ? install_sndh_timer_b :
		 timer.type == SNDH_TIMER_C ? install_sndh_timer_c :
		 timer.type == SNDH_TIMER_D ? install_sndh_timer_d :
					      install_sndh_vbl);

	return install_sndh_timer(timer.frequency);
}

static void idle(void)
{
	__asm__ __volatile__ ("stop #0x2200" : : : "cc");
}

static void idle_indefinitely(void)
{
	for (;;)
		idle();
}

void start(size_t size, void *sndh, u32 track, u32 timer)
{
	record = (struct record) { .size = size, .sndh = sndh };

	if (install_sndh_timer(u32_to_sndh_timer(timer)))
		sndh_init(track);

	idle_indefinitely();
}
