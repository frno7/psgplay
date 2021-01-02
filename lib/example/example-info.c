// SPDX-License-Identifier: GPL-2.0
/*
 * Simple example on how to display SNDH tags using sndh_for_each_tag.
 */

#include <stdlib.h>
#include <stdio.h>

#include "internal/print.h"

#include "psgplay/sndh.h"

#include "system/unix/file.h"

int main(int argc, char *argv[])
{
	if (argc <= 1) {
		fprintf(stderr, "usage: example-info <sndh-file>...\n");

		return EXIT_FAILURE;
	}

	for (int arg = 1; arg < argc; arg++) {
		const char *path = argv[arg];
		struct file f = sndh_read_file(path);

		if (!file_valid(f))
			pr_fatal_errno(path);

		sndh_for_each_tag (f.data, f.size)
			printf("%s %s\n", sndh_tag_name, sndh_tag_value);

		file_free(f);
	}

	return EXIT_SUCCESS;
}
