// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2025 Fredrik Noring
 */

#ifndef PSGPLAY_TEST_TEMPO_H
#define PSGPLAY_TEST_TEMPO_H

struct timer_preset {
	int ctrl;
	int divisor;
	int count;
};

#define T(c, d, n) { .ctrl = (c), .divisor = (d), .count = (n) }

#define tune_value_time_names(t)					\
	t(T(7, 200, 11), 63, "1117 Hz timer A")				\
	t(T(6, 100, 17), 63, "1445 Hz timer A")				\
	t(T(5,  64, 19), 63, "2021 Hz timer A")				\
	t(T(4,  50, 23), 63, "2137 Hz timer A")				\
	t(T(3,  16, 47), 63, "3268 Hz timer A")				\
	t(T(2,  10, 53), 63, "4636 Hz timer A")				\
	t(T(1,   4, 73), 63, "8416 Hz timer A")

#endif /* PSGPLAY_TEST_TEMPO_H */
