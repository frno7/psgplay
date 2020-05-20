// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "internal/macro.h"
#include "internal/string.h"

#include "psgplay/sndh.h"

#define diag_warn(cursor, ...)						\
	(cursor)->diag.warn((cursor)->diag.arg, __VA_ARGS__)

#define diag_error(cursor, ...)						\
	(cursor)->diag.error((cursor)->diag.arg, __VA_ARGS__)

static size_t sndh_head_offset(const void *data, const size_t size)
{
	const char *c = data;

	/*
	 * 20 bytes is the smallest possible header with "SNDH" at offset 12
	 * immediately followed by "HDNS".
	 */
	return size >= SNDH_PROLOGUE_SIZE &&
	       c[12] == 'S' &&
	       c[13] == 'N' &&
	       c[14] == 'D' &&
	       c[15] == 'H' ? 16 : 0;
}

bool sndh_identify(const void *data, size_t size)
{
	return sndh_head_offset(data, size) != 0;
}

static bool sndh_tag(const char *name, struct sndh_cursor *cursor)
{
	const size_t len = strlen(name);
	const char *c = cursor->file.data;

	if (cursor->offset + len > cursor->bound)
		return false;
	if (strncmp(name, &c[cursor->offset], len) != 0)
		return false;

	cursor->offset += len;

	return true;
}

static void tag_update(const char *value, const int integer,
	struct sndh_cursor *cursor)
{
	cursor->value = value;

	cursor->integer = integer;
}

static bool sndh_nul(struct sndh_cursor *cursor)
{
	const char *c = cursor->file.data;

	while (cursor->offset < cursor->bound)
		if (c[cursor->offset++] == '\0')
			return true;

	diag_warn(cursor, "tag %s missing NUL termination", cursor->tag->name);

	return false;
}

static bool sndh_string__(struct sndh_cursor *cursor)
{
	const char *c = cursor->file.data;

	if (!cursor->tag->length) {
		cursor->value = &c[cursor->offset];

		if (!sndh_nul(cursor))
			return false;

		return true;
	}

	if (cursor->bound < cursor->offset + cursor->tag->length + 1)
		return false;

	if (sizeof(cursor->buffer) < cursor->tag->length + 1) {
		diag_error(cursor, "tag %s too large for %zu",
			cursor->tag->name, sizeof(cursor->buffer));

		return false;
	}

	memcpy(cursor->buffer, &c[cursor->offset], cursor->tag->length);
	cursor->buffer[cursor->tag->length] = 0;
	cursor->value = cursor->buffer;
	cursor->offset += cursor->tag->length;

	if (c[cursor->offset] == '\0')
		(cursor->offset)++;
	else
		diag_warn(cursor, "tag %s missing NUL termination",
			cursor->tag->name);

	return true;
}

static bool sndh_string(struct sndh_cursor *cursor)
{
	if (!sndh_string__(cursor))
		return false;

	tag_update(cursor->value, 0, cursor);

	return true;
}

static size_t sndh_substrings_subend(size_t o, struct sndh_cursor  *cursor)
{
	const char *c = cursor->file.data;

	while (o < cursor->bound)
		if (c[o++] == '\0')
			return o;

	diag_warn(cursor, "tag %s missing NUL termination", cursor->tag->name);

	return 0;
}

static bool sndh_substrings_subtag(struct sndh_cursor  *cursor)
{
	const char *c = cursor->file.data;
	const uint8_t *b = cursor->file.data;
	const uint8_t *d = &b[cursor->offset];
	const size_t o = cursor->subtag.start + ((d[0] << 8) | d[1]);
	const size_t e = sndh_substrings_subend(o, cursor);

	if (!e)
		return false;

	tag_update(&c[o], 0, cursor);

	cursor->offset += 2;

	if (cursor->offset == cursor->subtag.bound)
		cursor->offset = cursor->subtag.bound = e;

	return true;
}

static bool sndh_substrings(struct sndh_cursor  *cursor)
{
	if (!cursor->subtunes) {
		diag_warn(cursor, "tag %s without any subtunes",
			cursor->tag->name);

		return true;
	}

	cursor->subtag.start = cursor->offset - strlen(cursor->tag->name);
	cursor->subtag.bound = cursor->offset + cursor->subtunes * 2;

	if (cursor->bound < cursor->subtag.bound) {
		diag_error(cursor, "tag %s too short", cursor->tag->name);

		return false;
	}

	cursor->subtag.read = sndh_substrings_subtag;

	return cursor->subtag.read(cursor);
}

static bool sndh_integer(struct sndh_cursor *cursor)
{
	int integer;

	if (!sndh_string__(cursor))
		return false;

	if (!strtoint(&integer, cursor->value, 10)) {
		diag_warn(cursor, "tag %s malformed number: %s",
			cursor->tag->name, cursor->value);

		return false;
	}

	snprintf(cursor->buffer, sizeof(cursor->buffer), "%d", integer);
	tag_update(cursor->buffer, integer, cursor);

	return true;
}

static bool sndh_subtunes(struct sndh_cursor *cursor)
{
	if (!sndh_string__(cursor))
		return false;

	if (!strtoint(&cursor->subtunes, cursor->value, 10)) {
		diag_warn(cursor, "tag %s malformed number: %s",
			cursor->tag->name, cursor->value);

		return false;
	}

	snprintf(cursor->buffer, sizeof(cursor->buffer), "%d", cursor->subtunes);
	tag_update(cursor->buffer, cursor->subtunes, cursor);

	return true;
}

static bool sndh_time_subtag(struct sndh_cursor *cursor)
{
	const uint8_t *b = cursor->file.data;
	const uint8_t *d = &b[cursor->offset];
	const int t = (d[0] << 8) | d[1];

	snprintf(cursor->buffer, sizeof(cursor->buffer), "%d", t);
	tag_update(cursor->buffer, t, cursor);

	cursor->offset += 2;

	return true;
}

static bool sndh_time(struct sndh_cursor *cursor)
{
	if (!cursor->subtunes) {
		diag_warn(cursor, "tag %s without any subtunes",
			cursor->tag->name);

		return true;
	}

	cursor->subtag.start = cursor->offset - strlen(cursor->tag->name);
	cursor->subtag.bound = cursor->offset + cursor->subtunes * 2;

	if (cursor->bound < cursor->subtag.bound) {
		diag_error(cursor, "tag %s too short", cursor->tag->name);

		return false;
	}

	cursor->subtag.read = sndh_time_subtag;

	return cursor->subtag.read(cursor);
}

static bool sndh_hdns(struct sndh_cursor *cursor)
{
	cursor->hdns = true;

	return true;
}

static bool sndh_padding(struct sndh_cursor *cursor)
{
	const char *c = cursor->file.data;

	if (cursor->offset < cursor->file.size && c[cursor->offset] == '\0') {
		cursor->offset++;

		return true;
	}

	return false;
}

static size_t branch_address(size_t offset, const void *data, size_t size)
{
	const uint8_t *b = data;

	if (offset + size < 2)
		return 0;

	const uint16_t w = (b[offset + 0] << 8) | b[offset + 1];
	if (w == 0x4e71)	/* nop */
		return branch_address(offset + 2, data, size);

	if ((w & 0xff00) == 0x6000 && (w & 0xff) != 0)		/* bra.s */
		return offset + (w & 0xff);

	if (offset + size < 4)
		return 0;

	if (w == 0x6000 ||	/* bra.w */
	    w == 0x4efa)	/* jmp(pc) */
		return offset + 2 + ((b[offset + 2] << 8) | b[offset + 3]);

	return 0;
}

static void branch_bound(size_t *bound,
	size_t offset, const void *data, size_t size)
{
	const size_t a = branch_address(offset, data, size);

	if (a < *bound)
		*bound = a;
}

static size_t tag_bound(const void *data, const size_t size)
{
	size_t bound = size;

	branch_bound(&bound, 0, data,  4);
	branch_bound(&bound, 4, data,  8);
	branch_bound(&bound, 8, data, 12);

	return bound;
}

static bool match_tag(struct sndh_cursor *cursor)
{
	static const struct sndh_tag tags[] = {
		{ "TITL",  sndh_string      },
		{ "COMM",  sndh_string      },
		{ "RIPP",  sndh_string      },
		{ "CONV",  sndh_string      },
		{ "YEAR",  sndh_string      },
		{ "TA",    sndh_integer     },
		{ "TB",    sndh_integer     },
		{ "TC",    sndh_integer     },
		{ "TD",    sndh_integer     },
		{ "!#SN",  sndh_substrings  },
		{ "!#",    sndh_integer     },
		{ "!V",    sndh_integer     },
		{ "##",    sndh_subtunes, 2 },
		{ "TIME",  sndh_time        },
		{ "FLAG~", sndh_string      },
		{ "FLAG",  sndh_substrings  },
		{ "HDNS",  sndh_hdns        },
		{ "",      sndh_padding     }
	};

	if (cursor->subtag.read && cursor->offset < cursor->subtag.bound)
		return cursor->subtag.read(cursor);
	else
		cursor->subtag.read = NULL;

restart:
	for (size_t i = 0; i < ARRAY_SIZE(tags); i++)
		if (sndh_tag(tags[i].name, cursor)) {
			cursor->tag = &tags[i];

			if (cursor->tag->read == sndh_padding) {
				if (!tags[i].read(cursor))
					return false;

				goto restart;
			}

			return tags[i].read(cursor);
		}

	return false;
}

size_t sndh_header_size(const void *data, const size_t size)
{
	size_t header_size;

	sndh_for_each_tag_with_header_size (data, size, &header_size);

	return header_size;
}

static void diag_warn_ignore(void *arg, const char *fmt, ...) { }
static void diag_error_ignore(void *arg, const char *fmt, ...) { }

struct sndh_cursor sndh_first_tag(const void *data, const size_t size,
	size_t *header_size, const struct sndh_diagnostic *diag)
{
	size_t offset = sndh_head_offset(data, size);

	if (header_size)
		*header_size = 0;

	if (!offset) {
		if (diag)
			diag->error(diag->arg, "invalid SNDH header");

		return (struct sndh_cursor) { };
	}

	struct sndh_cursor cursor = {
		.file = {
			.size = size,
			.data = data
		},

		.header = {
			.size = header_size
		},

		.bound = tag_bound(data, size),
		.offset = offset,

		.valid = true,

		.subtunes = 1,	/* Default to 1, unless the ## tag is given */

		.diag = {
			.warn  = diag ? diag->warn  : diag_warn_ignore,
			.error = diag ? diag->error : diag_error_ignore,
			.arg   = diag ? diag->arg   : NULL
		}
	};

	sndh_next_tag(&cursor);

	return cursor;
}

bool sndh_valid_tag(const struct sndh_cursor *cursor)
{
	if (cursor->hdns)
		return false;

	if (!cursor->valid)
		return false;

	if (cursor->bound <= cursor->offset) {
		diag_warn(cursor, "missing HDNS tag");

		return false;
	}

	return true;
}

void sndh_next_tag(struct sndh_cursor *cursor)
{
	const bool valid = match_tag(cursor);

	if (cursor->header.size)
		*cursor->header.size = cursor->offset;

	if (!valid) {
		cursor->valid = false;

		if (cursor->offset == cursor->bound)
			diag_warn(cursor, "missing HDNS tag");
		else
			diag_error(cursor,
				"unrecognised data in %zu bytes at offset %zu",
				cursor->bound - cursor->offset, cursor->offset);
	}
}

bool sndh_tag_subtune_count(int *subtune_count,
	const void *data, const size_t size)
{
	sndh_for_each_tag (data, size)
		if (strcmp(sndh_tag_name, "##") == 0) {
			*subtune_count = sndh_tag_integer;

			return true;
		}

	return false;
}

bool sndh_tag_default_subtune(int *default_subtune,
	const void *data, const size_t size)
{
	sndh_for_each_tag (data, size)
		if (strcmp(sndh_tag_name, "!#") == 0) {
			*default_subtune = sndh_tag_integer;

			return true;
		}

	return false;
}

bool sndh_tag_subtune_time(float *duration, int subtune,
	const void *data, const size_t size)
{
	int st = 0;

	sndh_for_each_tag (data, size)
		if (strcmp(sndh_tag_name, "TIME") == 0 && ++st == subtune) {
			*duration = sndh_tag_integer;

			return true;
		}

	return false;
}

bool sndh_tag_timer(struct sndh_timer *timer,
	const void *data, const size_t size)
{
	static const struct {
		const char *name;
		enum sndh_timer_type type;
	} timers[] = {
		{ "TA", SNDH_TIMER_A },
		{ "TB", SNDH_TIMER_B },
		{ "TC", SNDH_TIMER_C },
		{ "TD", SNDH_TIMER_D },
		{ "!V", SNDH_TIMER_V },
	};

	sndh_for_each_tag (data, size)
		for (size_t i = 0; i < ARRAY_SIZE(timers); i++)
			if (strcmp(sndh_tag_name, timers[i].name) == 0) {
				*timer = (struct sndh_timer) {
					.type = timers[i].type,
					.frequency = sndh_tag_integer
				};

				return true;
			}

	return false;
}

bool sndh_tag_title(char *title, size_t length,
	const void *data, const size_t size)
{
	if (!length)
		return false;

	sndh_for_each_tag (data, size)
		if (strcmp(sndh_tag_name, "TITL") == 0) {
			strncpy(title, sndh_tag_value, length - 1);
			title[length - 1] = '\0';

			return true;
		}

	return false;
}

size_t sndh_init_address(const void *data, const size_t size)
{
	return size >= 4 ? branch_address(0, data, 4) : 0;
}

size_t sndh_exit_address(const void *data, const size_t size)
{
	return size >= 8 ? branch_address(4, data, 8) : 0;
}

size_t sndh_play_address(const void *data, const size_t size)
{
	return size >= 12 ? branch_address(8, data, 12) : 0;
}
