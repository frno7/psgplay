// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#ifndef TOS_STDREG_S
#define TOS_STDREG_S

#define REG_(r) % ## r

#define d0 REG_(d0)
#define d1 REG_(d1)
#define d2 REG_(d2)
#define d3 REG_(d3)
#define d4 REG_(d4)
#define d5 REG_(d5)
#define d6 REG_(d6)
#define d7 REG_(d7)
#define a0 REG_(a0)
#define a1 REG_(a1)
#define a2 REG_(a2)
#define a3 REG_(a3)
#define a4 REG_(a4)
#define a5 REG_(a5)
#define a6 REG_(a6)
#define sp REG_(sp)
#define sr REG_(sr)
#define pc REG_(pc)

#endif /* TOS_STDREG_S */
