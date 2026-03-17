// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#ifndef ATARI_M68K_H
#define ATARI_M68K_H

struct m68k_module;

int m68k_int_ack_callback(struct m68k_module *module, int level);

void m68k_instruction_callback(struct m68k_module *module, int pc);

#endif /* ATARI_M68K_H */
