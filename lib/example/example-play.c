// SPDX-License-Identifier: GPL-2.0
/*
 * Simple example on how to play an SNDH file in 44.1 kHz stereo on
 * standard output. Pipe output to for example the aplay command
 *
 * 	lib/psgplay/example-play example.sndh | aplay -r44100 -c2 -fS16_LE
 *
 * or -fS16_BE for a big-endian machine.
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "internal/macro.h"
#include "internal/print.h"

#include "psgplay/psgplay.h"

#include "system/unix/file.h"

int main(int argc, char *argv[])
{
	if (argc <= 1) {
		fprintf(stderr, "usage: example-play <sndh-file>...\n");

		return EXIT_FAILURE;
	}

	for (int arg = 1; arg < argc; arg++) {
		const char *path = argv[arg];
		struct file f = sndh_read_file(path);

		if (!file_valid(f))
			pr_fatal_errno(path);

		struct psgplay *pp = psgplay_init(f.data, f.size, 1, 44100);
		if (!pp)
			pr_fatal_error("%s: Failed to play file\n", path);

		for (;;) {
			struct psgplay_stereo buffer[4096];

			const ssize_t r = psgplay_read_stereo(pp,
				buffer, ARRAY_SIZE(buffer));

			if (r <= 0)
				break;

			const ssize_t s = sizeof(struct psgplay_stereo[r]);
			const ssize_t w = xwrite(STDOUT_FILENO, buffer, s);

			if (w != s)
				break;
		}

		psgplay_free(pp);

		file_free(f);
	}

	return EXIT_SUCCESS;
}
