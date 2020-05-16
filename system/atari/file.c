// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "internal/compare.h"

#include "psgplay/ice.h"
#include "psgplay/sndh.h"

#include "system/atari/file.h"
#include "system/atari/ice_decrunch_inplace.h"

#define SNDH_CHUNK_SIZE 1024	/* FIXME: Number of bytes before yielding */

ssize_t fsize(int fd)
{
	const off_t org = lseek(fd, 0, SEEK_CUR);
	if (org == -1)
		return -1;

	const ssize_t size = lseek(fd, 0, SEEK_END);
	if (size == -1)
		return -1;

	if (lseek(fd, org, SEEK_SET) == -1)
		return -1;

	return size;
}

bool file_valid(struct file *file)
{
	return file->path != NULL;
}

bool sndh_read_first(const char *path, struct file *file,
	struct file_cursor *file_cursor)
{
	u8 prologue[max(ICE_HEADER_SIZE, SNDH_PROLOGUE_SIZE)];
	void *data = NULL;

	*file = (struct file) { };
	*file_cursor = (struct file_cursor) { };

	int fd = open(path, O_RDONLY);
	if (fd == -1)
		return false;

	const ssize_t file_size = fsize(fd);
	if (file_size == -1)
		goto err_out;
	if (file_size < sizeof(prologue)) {
		errno = -ENOEXEC;
		goto err_out;
	}

	ssize_t r = read(fd, prologue, sizeof(prologue));
	if (r == -1)
		goto err_out;
	if (r != sizeof(prologue)) {
		errno = -ENOEXEC;
		goto err_out;
	}

	bool ice = false;
	size_t final_size;
	if (ice_identify(prologue, sizeof(prologue))) {
		final_size = ice_decrunched_size(prologue, sizeof(prologue));
		ice = true;
	} else if (sndh_identify(prologue, sizeof(prologue))) {
		final_size = file_size;
	} else {
		errno = -ENOEXEC;
		goto err_out;
	}

	data = malloc(final_size + 1);	/* +1 for final NUL */
	if (!data)
		goto err_out;

	memcpy(data, prologue, sizeof(prologue));

	*file = (struct file) {
		.path = path,
		.size = final_size,
		.data = data
	};
	*file_cursor = (struct file_cursor) {
		.file = file,
		.file_offset = sizeof(prologue),
		.file_size = file_size,
		.fd = fd,
		.ice = {
			.crunched = ice,
		},
	};

	return true;

err_out:
	preserve (errno) {
		free(data);
		close(fd);
	}

	return false;
}

bool sndh_read_finished(struct file_cursor *file_cursor)
{
	if (!file_cursor->file)
		return true;

	if (file_cursor->ice.crunched)
		return file_cursor->ice.offset == file_cursor->file->size;

	return file_cursor->file_offset == file_cursor->file_size;
}

int sndh_read_progress(struct file_cursor *file_cursor)
{
	if (file_cursor->ice.crunched) {
		/*
		 * Weighted file and decompressed size and total to
		 * compensate for the difference in speed between
		 * loading files and decrunching data.
		 */
		const size_t ws =
			(70 * file_cursor->file_offset) +
			(30 * file_cursor->ice.offset);
		const size_t wt =
			(70 * file_cursor->file_size) +
			(30 * file_cursor->file->size);

		return wt ? (100 * ws) / wt : 100;
	}

	return (100 * file_cursor->file_offset) / file_cursor->file_size;
}

bool sndh_read_next(struct file_cursor *file_cursor)
{
	if (!file_cursor->file)
		return false;

	if (file_cursor->file_offset < file_cursor->file_size) {
		const size_t size = min(SNDH_CHUNK_SIZE,
			file_cursor->file_size - file_cursor->file_offset);
		u8 *b = file_cursor->file->data;
		u8 *d = &b[file_cursor->file_offset];

		ssize_t r = read(file_cursor->fd, d, size);
		if (r == -1)
			goto err_out;
		if (r != size) {
			errno = -EIO;
			goto err_out;
		}

		file_cursor->file_offset += r;
	}

	if (file_cursor->fd != -1 &&
			file_cursor->file_offset == file_cursor->file_size) {
		const int err = close(file_cursor->fd);

		file_cursor->fd = -1;

		if (err == -1)
			goto err_out;

		if (file_cursor->ice.crunched) {
			file_cursor->ice.context = ice_decrunch_inplace_init(
				file_cursor->file->data,
				file_cursor->file->size);
		}
	}

	if (file_cursor->fd == -1) {
		if (file_cursor->ice.crunched &&
				file_cursor->ice.offset !=
				file_cursor->file->size)
			ice_decrunch_inplace_partial(
				file_cursor->file->data,
				&file_cursor->ice.context,
				&file_cursor->ice.offset);

		if (!file_cursor->ice.crunched ||
				file_cursor->ice.offset ==
				file_cursor->file->size) {
			u8 *b = file_cursor->file->data;

			b[file_cursor->file->size] = '\0'; /* Apply final NUL */
		}
	}

	return true;

err_out:
	preserve (errno) {
		free(file_cursor->file->data);

		if (file_cursor->fd != -1)
			close(file_cursor->fd);
	}

	*file_cursor->file = (struct file) { };
	*file_cursor = (struct file_cursor) { };

	return false;
}

const char *file_basename(const char *path)
{
	size_t k = 0;

	for (size_t i = 0; path[i]; i++)
		if (path[i] == '\\')
			k = i + 1;

	return &path[k];
}
