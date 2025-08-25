// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2025 Fredrik Noring
 */

#ifndef PSGPLAY_TEST_PSGPITCH_H
#define PSGPLAY_TEST_PSGPITCH_H

#define tune_value_names(t)						\
	t(  440, "PSG square wave A4 pitch standard A440 440 Hz")	\
	t(   33, "PSG square wave C1 double low C 33 Hz")		\
	t(   65, "PSG square wave C2 low C (cello) 65 Hz")		\
	t(  131, "PSG square wave C3 tenor C (organ) 131 Hz")		\
	t(  262, "PSG square wave C4 middle C 262 Hz")			\
	t(  523, "PSG square wave C5 treble C 523 Hz")			\
	t( 1047, "PSG square wave C6 high C (soprano) 1047 Hz")		\
	t( 2093, "PSG square wave C7 double high C 2093 Hz")		\
	t( 4186, "PSG square wave C8 triple high C 4186 Hz")		\
	t( 8372, "PSG square wave C9 quadruple high C 8372 Hz")		\
	t(16744, "PSG square wave C10 quintuple high C 16744 Hz")

#endif /* PSGPLAY_TEST_PSGPITCH_H */
