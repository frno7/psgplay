// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "internal/compare.h"
#include "internal/macro.h"
#include "internal/print.h"
#include "internal/types.h"

#include "psgplay/ice.h"

#include "system/unix/file.h"
#include "system/unix/memory.h"
#include "system/unix/string.h"

static struct file file_read_fd__(int fd, char *path)
{
	size_t size = 0;
	u8 *data = NULL;

	if (fd < 0)
		goto err;

	for (;;) {
		const size_t capacity =
			size + clamp_val(size, 0x1000, 0x100000);

		void * const d = realloc(data, capacity);
		if (!d)
			goto err;
		data = d;

		const ssize_t r = xread(fd, &data[size], capacity - size);
		if (!r) {
			data[size] = '\0';	/* Always NUL terminate */
			break;
		}
		if (r == -1)
			goto err;

		size += r;
	}

	if (xclose(fd) == -1) {
		fd = -1;
		goto err;
	} else
		fd = -1;

	void * const d = realloc(data, size + 1);	/* +1 for NUL */
	if (!d)
		goto err;

	return (struct file) {
		.path = path,
		.size = size,
		.data = d
	};

err:
	preserve (errno) {
		if (fd >= 0)
			xclose(fd);

		free(data);
		free(path);
	}

	return (struct file) { };
}

struct file file_read(const char *path)
{
	return file_read_fd__(xopen(path, O_RDONLY), xstrdup(path));
}

struct file file_read_or_stdin(const char *path)
{
	return strcmp(path, "-") == 0 ?
		file_read_fd(STDIN_FILENO, path) : file_read(path);
}

struct file file_read_fd(int fd, const char *path)
{
	return file_read_fd__(fd, xstrdup(path));
}

void file_free(struct file f)
{
	free(f.path);
	free(f.data);
}

bool file_valid(struct file f)
{
	return f.path != NULL;
}

int xopen(const char *path, int oflag, ...)
{
        mode_t mode = 0;
        va_list ap;

        va_start(ap, oflag);
        if (oflag & O_CREAT)
                mode = va_arg(ap, int);
        va_end(ap);

	do {
                const int fd = open(path, oflag, mode);

                if (fd >= 0)
                        return fd;
	} while (errno == EINTR);

	return -1;
}

int xclose(int fd)
{
	int err;

	do {
		err = close(fd);
	} while (err == EINTR);

	return err < 0 ? -1 : 0;
}

ssize_t xread(int fd, void *buf, size_t nbyte)
{
	u8 *data = buf;
	size_t size = 0;

	while (size < nbyte) {
		const ssize_t r = read(fd, &data[size], nbyte - size);

		if (r < 0) {
			if (errno == EINTR)
				continue;
			return -1;
		} else if (!r)
			return size;

		size += r;
	}

	return size;
}

ssize_t xwrite(int fd, const void *buf, size_t nbyte)
{
	const u8 *data = buf;
	size_t size = 0;

	while (size < nbyte) {
		const ssize_t w = write(fd, &data[size], nbyte - size);

		if (w < 0) {
			if (errno == EINTR)
				continue;
			return -1;
		} else if (!w)
			return size;

		size += w;
	}

	return size;
}

void file_nonblocking(int fd)
{
	if (fcntl(fd, F_SETFL, O_NONBLOCK) == -1)
		pr_fatal_errno("nonblocking:fcntl");
}

const char *file_basename(const char *path)
{
	size_t k = 0;

	for (size_t i = 0; path[i]; i++)
		if (path[i] == '/')
			k = i + 1;

	return &path[k];
}

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
