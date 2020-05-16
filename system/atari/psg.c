// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#include <stdbool.h>
#include <stdlib.h>

#include <tos/system-variable.h>
#include <tos/xbios.h>

#include "atari/psg.h"

#include "system/atari/psg.h"

static bool key_click;

static void save_disable_key_click(void)
{
	key_click = __system_variables->conterm.key_click;

	__system_variables->conterm.key_click = false;
}

static void restore_key_click(void)
{
	__system_variables->conterm.key_click = key_click;
}

static void psg_exit(void)
{
	psg_mute();

	xbios_supexec(restore_key_click);
}

void psg_init(void)
{
	xbios_supexec(save_disable_key_click);

	atexit(psg_exit);
}

u8 psg_mute(void)
{
	const u8 iomix = xbios_giaccess(0, PSG_REG_IOMIX);

	xbios_giaccess(0x3f | iomix, XBIOS_GIACCESS_SET | PSG_REG_IOMIX);

	return iomix;
}

void psg_unmute(u8 iomix)
{
	xbios_giaccess(iomix, XBIOS_GIACCESS_SET | PSG_REG_IOMIX);
}
