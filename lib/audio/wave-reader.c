// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2025 Fredrik Noring
 */

#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "internal/assert.h"
#include "internal/compare.h"
#include "internal/print.h"

#include "audio/wave-reader.h"

#include "system/unix/file.h"
#include "system/unix/memory.h"

typedef char wave_str[4];
typedef u8 wave_u16[2];
typedef u8 wave_u32[4];

struct wave_sample {
	u8 lo;
	s8 hi;
};

struct wave_state {
	const char *path;
	int fd;

	struct audio_format format;
};

#define WAVE_STR_EQ(a, b) ((a)[0] == (b)[0] && \
			   (a)[1] == (b)[1] && \
			   (a)[2] == (b)[2] && \
			   (a)[3] == (b)[3])
#define WAVE_U16(u) ((u)[0] | ((u)[1] << 8))
#define WAVE_U32(u) ((u)[0] | ((u)[1] << 8) | ((u)[2] << 16) | ((u)[3] << 24))

struct wave_riff {
	wave_str ckid;			/* "RIFF" */
	wave_u32 cksize;		/* File size - 8 */
	wave_str waveid;		/* "WAVE" */
};

struct wave_chunk {
	wave_str ckid;			/* "fmt " */
	wave_u32 cksize;		/* Frame size, 16 */
	wave_u16 format;		/* Format tag, 1 PCM */
	wave_u16 n_channels;		/* Number of channels */
	wave_u32 sample_rate;		/* Samples per second */
	wave_u32 avg_byte_rate;		/* Average bytes per second */
	wave_u16 bytes_per_sample;	/* Bytes per sample */
	wave_u16 bits_per_sample;	/* Bits per sample */
};

struct wave_data {
	wave_str ckid;			/* "data" */
	wave_u32 cksize;		/* File size - 44 */
};

static void wave_read_header(struct wave_state *state)
{
	struct {
		struct wave_riff riff;
		struct wave_chunk chunk;
		struct wave_data data;
	} wave_header;

	BUILD_BUG_ON(sizeof(wave_header) != 44);

	const ssize_t size = xread(state->fd, &wave_header, sizeof(wave_header));
	if (size == -1)
		pr_fatal_errno(state->path);
	if (size != sizeof(wave_header))
		pr_fatal_error("%s: Failed to read complete WAVE header\n",
			state->path);

	if (!WAVE_STR_EQ(wave_header.riff.ckid, "RIFF"))
		pr_fatal_error("%s: WAVE header missing \"RIFF\" field\n",
			state->path);
	if (!WAVE_STR_EQ(wave_header.riff.waveid, "WAVE"))
		pr_fatal_error("%s: WAVE header missing \"WAVE\" field\n",
			state->path);

	if (!WAVE_STR_EQ(wave_header.chunk.ckid, "fmt "))
		pr_fatal_error("%s: WAVE header missing \"fmt \" field\n",
			state->path);
	if (WAVE_U32(wave_header.chunk.cksize) != 16)
		pr_fatal_error("%s: WAVE chunk size (%u) not 16 bytes\n",
			state->path, WAVE_U32(wave_header.chunk.cksize));
	if (WAVE_U16(wave_header.chunk.format) != 1)
		pr_fatal_error("%s: WAVE field format (%d) not PCM\n",
			state->path, WAVE_U16(wave_header.chunk.format));
	if (WAVE_U16(wave_header.chunk.n_channels) != 2)
		pr_fatal_error("%s: WAVE field n_channels (%d) not stereo\n",
			state->path, WAVE_U16(wave_header.chunk.n_channels));
	if (WAVE_U16(wave_header.chunk.bytes_per_sample) != 4)
		pr_fatal_error("%s: WAVE field bytes_per_sample (%d) not 4\n",
			state->path, WAVE_U16(wave_header.chunk.bytes_per_sample));
	if (WAVE_U16(wave_header.chunk.bits_per_sample) != 16)
		pr_fatal_error("%s: WAVE field bits_per_sample (%d) not 16\n",
			state->path, WAVE_U16(wave_header.chunk.bits_per_sample));

	if (!WAVE_STR_EQ(wave_header.data.ckid, "data"))
		pr_fatal_error("%s: WAVE header missing \"data\" field\n",
			state->path);

	state->format.frequency = WAVE_U32(wave_header.chunk.sample_rate);
	state->format.sample_count = WAVE_U32(wave_header.data.cksize) / 4;
}

static size_t wave_sample(struct audio_sample *samples, size_t count, void *arg)
{
	struct wave_state *state = arg;

	const ssize_t size = xread(state->fd, samples, 4 * count);
	if (size == -1)
		pr_fatal_errno(state->path);

	return size / 4;
}

static struct audio_format wave_format(void *arg)
{
	struct wave_state *state = arg;

	return state->format;
}

static void *wave_open(const char *path)
{
	struct wave_state *state = xmalloc(sizeof(struct wave_state));

	*state = (struct wave_state) {
		.path = path,
		.fd = xopen(path, O_RDONLY),
	};

	if (state->fd == -1)
		pr_fatal_errno(path);

	wave_read_header(state);

	return state;
}

static void wave_close(void *arg)
{
	struct wave_state *state = arg;

	if (xclose(state->fd) == -1)
		pr_fatal_errno(state->path);

	free(state);
}

const struct audio_reader wave_reader = {
	.open	= wave_open,
	.format = wave_format,
	.sample	= wave_sample,
	.close	= wave_close,
};
