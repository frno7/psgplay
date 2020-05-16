// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#ifndef PSGPLAY_SYSTEM_ATARI_FILE_H
#define PSGPLAY_SYSTEM_ATARI_FILE_H

#include <stdbool.h>
#include <stddef.h>

#include "system/atari/ice_decrunch_inplace.h"

ssize_t fsize(int fd);

/**
 * struct file - file container
 * @path: path of file
 * @size: size in bytes of file
 * @data: contents of file, always NUL terminated
 */
struct file {
	const char *path;
	size_t size;
	void *data;
};

struct file_cursor {
	struct file *file;
	size_t file_offset;
	size_t file_size;
	int fd;
	struct {
		bool crunched;
		size_t offset;
		struct ice_decrunch_inplace context;
	} ice;
};

bool file_valid(struct file *file);

bool sndh_read_first(const char *path, struct file *file,
	struct file_cursor *file_cursor);

bool sndh_read_next(struct file_cursor *file_cursor);

bool sndh_read_finished(struct file_cursor *file_cursor);

int sndh_read_progress(struct file_cursor *file_cursor);

const char *file_basename(const char *path);

#endif /* PSGPLAY_SYSTEM_ATARI_FILE_H */
