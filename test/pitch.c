// SPDX-License-Identifier: GPL-2.0

#include "test/psg.h"
#include "test/sndh.h"

/*
 * https://en.wikipedia.org/wiki/Scientific_pitch_notation
 * https://en.wikipedia.org/wiki/C_(musical_note)
 */
const int pitch_440_hz = 440;
const int pitch_262_hz = 262;

#define tunes(t)							\
	t(pitch_440_hz, "A4 pitch standard A440 440 Hz")		\
	t(pitch_262_hz, "C4 middle C 261.6256 Hz")

sndh_title("Pitch");
sndh_tune_names(int *, tunes);

void sndh_init(int tune)
{
#if 0	/*
	 * FIXME: What we want to do.
	 */
	const int pitch = *sndh_tune_select(tune);
#else	/*
	 * What has to be done to avoid position-independent code issue.
	 *
	 * The GCC linker option we seem to want is -static-pie, but it
	 * doesn't seem to be available with m68k. Perhaps we must link
	 * crt0.o for some kind of runtime relocation support?
	 *
	 * Other options that might be of interest: -mpcrel -fpie --pie
	 *
	 * https://gcc.gnu.org/onlinedocs/gcc/Link-Options.html#index-static-pie
	 * https://gcc.gnu.org/onlinedocs/gcc/Code-Gen-Options.html
	 * https://gcc.gnu.org/onlinedocs/gcc/M680x0-Options.html
	 */
	const int pitch = tune == 2 ? pitch_262_hz : pitch_440_hz;
#endif

	psg_wr_period_a(DIV_ROUND_CLOSEST_U32(PSG_FREQUENCY, 16 * pitch));
	psg_wr_level_a(15);
}

void sndh_play()
{
	psg_wr_iomix(0b111110);
}
