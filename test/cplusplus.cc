// SPDX-License-Identifier: GPL-2.0
/*
 * Simple C++ compile and link test.
 */

extern "C" {
#include "psgplay/psgplay.h"
#include "psgplay/stereo.h"
#include "psgplay/sndh.h"
}

int main()
{
	struct psgplay *pp = psgplay_init(NULL, 0, 1, 44100);

	psgplay_free(pp);

	return 0;
}
