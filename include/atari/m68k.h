// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#ifndef ATARI_M68K_H
#define ATARI_M68K_H

int m68k_int_ack_callback(int level);

void m68k_instruction_callback(int pc);

#endif /* ATARI_M68K_H */
