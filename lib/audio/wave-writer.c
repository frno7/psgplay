// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "internal/assert.h"
#include "internal/compare.h"
#include "internal/print.h"

#include "audio/wave-writer.h"

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
	const char *output;
	int fd;

	int frequency;

	size_t sample_count;

	size_t buffer_count;
	struct {
		struct wave_sample left;
		struct wave_sample right;
	} buffer[16384];
};

#define WAVE_STR(s) { (s)[0], (s)[1], (s)[2], (s)[3] }
#define WAVE_U16(n) { (n) & 0xff, ((n) >> 8) & 0xff }
#define WAVE_U32(n) { (n) & 0xff, ((n) >> 8) & 0xff, ((n) >> 16) & 0xff, ((n) >> 24) & 0xff }
#define WAVE_SAMPLE(s) ((struct wave_sample) { .lo = (s),.hi = (s) >> 8 })

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

static void wave_write_header(int fd, const char *output,
	int frequency, size_t sample_count)
{
	const size_t total_size =
		sizeof(struct wave_riff) +
		sizeof(struct wave_chunk) +
		sizeof(struct wave_data) + 4 * sample_count;

	const struct {
		struct wave_riff riff;
		struct wave_chunk chunk;
		struct wave_data data;
	} wave_header = {
		.riff = {
			.ckid			= WAVE_STR("RIFF"),
			.cksize			= WAVE_U32(total_size - 8),
			.waveid			= WAVE_STR("WAVE"),
		},
		.chunk = {
			.ckid			= WAVE_STR("fmt "),
			.cksize			= WAVE_U16(16),
			.format			= WAVE_U16(1),
			.n_channels		= WAVE_U16(2),
			.sample_rate		= WAVE_U32(frequency),
			.avg_byte_rate		= WAVE_U32(frequency * 4),
			.bytes_per_sample	= WAVE_U16(4),
			.bits_per_sample	= WAVE_U16(16),
		},
		.data = {
			.ckid			= WAVE_STR("data"),
			.cksize			= WAVE_U32(total_size - 44),
		},
	};

	BUILD_BUG_ON(sizeof(wave_header) != 44);
	BUG_ON((u32)total_size != total_size);

	if (lseek(fd, 0, SEEK_SET) == -1)
		pr_fatal_errno(output);

	const ssize_t size = xwrite(fd, &wave_header, sizeof(wave_header));
	if (size == -1)
		pr_fatal_errno(output);
	else if (size != sizeof(wave_header))
		pr_fatal_error("%s: Failed to write complete WAVE header\n",
			output);
}

static bool wave_sample_flush(struct wave_state *state)
{
	const size_t buffer_size = state->buffer_count * sizeof(*state->buffer);
	const ssize_t size = xwrite(state->fd, state->buffer, buffer_size);

	BUG_ON(4 * ARRAY_SIZE(state->buffer) != sizeof(state->buffer));

	state->buffer_count = 0;

	if (size == -1)
		pr_fatal_errno(state->output);
	else if (size != buffer_size)
		pr_fatal_error("%s: Failed to write complete WAVE samples\n",
			state->output);

	return true;
}

static bool wave_sample(s16 left, s16 right, void *arg)
{
	struct wave_state *state = arg;

	state->sample_count++;

	state->buffer[state->buffer_count].left  = WAVE_SAMPLE(left);
	state->buffer[state->buffer_count].right = WAVE_SAMPLE(right);
	state->buffer_count++;

	if (state->buffer_count < ARRAY_SIZE(state->buffer))
		return true;

	return wave_sample_flush(state);
}

static void *wave_open(const char *output, int frequency, bool nonblocking)
{
	struct wave_state *state = xmalloc(sizeof(struct wave_state));

	*state = (struct wave_state) {
		.output = output,
		.fd = xopen(output, O_WRONLY | O_CREAT | O_TRUNC, 0644),
		.frequency = frequency,
	};

	if (state->fd == -1)
		pr_fatal_errno(output);

	/* Provisional header to be updated on close. */
	wave_write_header(state->fd, output, frequency, 0);

	return state;
}

static void wave_close(void *arg)
{
	struct wave_state *state = arg;

	wave_sample_flush(state);

	/* Final header. */
	wave_write_header(state->fd, state->output,
		state->frequency, state->sample_count);

	if (xclose(state->fd) == -1)
		pr_fatal_errno(state->output);

	free(state);
}

const struct audio_writer wave_writer = {
	.open	= wave_open,
	.sample	= wave_sample,
	.close	= wave_close,
};
