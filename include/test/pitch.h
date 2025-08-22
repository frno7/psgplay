// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2025 Fredrik Noring
 */

#ifndef PSGPLAY_TEST_PITCH_H
#define PSGPLAY_TEST_PITCH_H

#define tune_value_names(t)						\
	t(  440, "Square wave A4 pitch standard A440 440 Hz")		\
	t(   33, "Square wave C1 double low C 33 Hz")			\
	t(   65, "Square wave C2 low C (cello) 65 Hz")			\
	t(  131, "Square wave C3 tenor C (organ) 131 Hz")		\
	t(  262, "Square wave C4 middle C 262 Hz")			\
	t(  523, "Square wave C5 treble C 523 Hz")			\
	t( 1047, "Square wave C6 high C (soprano) 1047 Hz")		\
	t( 2093, "Square wave C7 double high C 2093 Hz")		\
	t( 4186, "Square wave C8 triple high C 4186 Hz")		\
	t( 8372, "Square wave C9 quadruple high C 8372 Hz")		\
	t(16744, "Square wave C10 quintuple high C 16744 Hz")

#endif /* PSGPLAY_TEST_PITCH_H */
