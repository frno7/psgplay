// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#include <stdlib.h>

#include <tos/cookie.h>
#include <tos/system-variable.h>

#include "internal/macro.h"
#include "internal/build-assert.h"
#include "internal/check-compiler.h"
#include "internal/types.h"
#include "internal/limits.h"

#include "psgplay/sndh.h"

#include "atari/mfp-map.h"

struct timer_prescale {
	enum mfp_ctrl ctrl;
	u8 count;
};

struct system_variables *__system_variables = (struct system_variables *)0x400;

static struct cookie cookie_jar[16] = {
	{ .uid = COOKIE__CPU, .value = COOKIE__CPU_68000 },
	{ .uid = COOKIE__VDO, .value = COOKIE__VDO_STE   },
	{ .uid = COOKIE__MCH, .value = COOKIE__MCH_STE   },
	{ .uid = COOKIE__SWI, .value = 0xff              },
	{ .uid = COOKIE__SND, .value = COOKIE__SND_PSG
				     | COOKIE__SND_DMA8  },
	{ .uid = COOKIE__FPU, .value = COOKIE__FPU_NONE  },
	{ .value = __ARRAY_SIZE(cookie_jar) }
};

struct sndh_file file;

static struct {
	enum sndh_timer_type type;
	struct timer_prescale prescale;
} timer_state;

static u32 gemdos_malloc(const u32 amount)
{
	const u32 address = (__system_variables->_membot & ~(u32)0xf) + 0x10;

	__system_variables->_membot = address + amount;

	return address;
}

u32 gemdos_trap(u8 *sp)
{
	switch (*(u16*)sp) {
		case 72:
			return gemdos_malloc(*(u32*)&sp[2]);
	}

	return 0;
}

u32 xbios_trap()
{
	return 0;
}

static bool timer_prescale(struct timer_prescale *prescale, int frequency)
{
	static const u8 prescale_div[] = {
#define MFP_CTRL_DIV_PRESCALE(div) div,
MFP_CTRL_DIV(MFP_CTRL_DIV_PRESCALE)
	};
	bool valid = false;
	int best = -1;

	if (frequency < 1)
		return false;

	for (size_t i = 0; i < ARRAY_SIZE(prescale_div); i++) {
		if (frequency > U32_MAX / prescale_div[i])
			continue;

		const u32 count = DIV_ROUND_CLOSEST_U32(
			MFP_TIMER_FREQUENCY, prescale_div[i] * (u32)frequency);

		if (count < 1 || 256 < count)
			continue;

		const u32 f = DIV_ROUND_CLOSEST_U32(
			MFP_TIMER_FREQUENCY, prescale_div[i] * count);
		const int diff = abs(f - frequency);

		if (best >= 0 && diff >= best)
			continue;

		best = diff;
		valid = true;
		*prescale = (struct timer_prescale) {
			.ctrl = 1 + i,
			.count = count & 0xff,
		};
	}

	return valid;
}

void vbl_exception(void)
{
	if (timer_state.type == SNDH_TIMER_V)
		sndh_play(&file);
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
			sndh_play(&file);				\
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
	BUILD_BUG_ON(sizeof(struct system_variables) != 0x1b4);

	file = (struct sndh_file) { .size = size, .sndh = sndh };

	__system_variables->_membot = (u32)sndh + size + 1024;
	__system_variables->_p_cookies = cookie_jar;

	if (install_sndh_timer(u32_to_sndh_timer(timer)))
		sndh_init(track, &file);

	idle_indefinitely();
}
