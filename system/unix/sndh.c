// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#include <errno.h>
#include <stdlib.h>

#include "internal/print.h"

#include "ice/ice.h"

#include "system/unix/file.h"
#include "system/unix/memory.h"
#include "system/unix/sndh.h"

struct file sndh_read_file(const char *path)
{
	struct file file = file_read_or_stdin(path);

	if (!file_valid(file))
		return file;

	if (ice_identify(file.data, file.size)) {
		const size_t s = ice_decrunched_size(file.data, file.size);
		void *b = xmalloc(s);

		if (ice_decrunch(b, file.data, file.size) == -1) {
			pr_error("%s: ICE decrunch failed\n", file.path);

			free(b);
			file_free(file);
			errno = ENOEXEC;

			return (struct file) { };
		}

		free(file.data);
		file.size = s;
		file.data = b;
	}

	return file;
}
