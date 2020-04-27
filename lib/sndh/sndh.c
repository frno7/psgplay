// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#include <errno.h>
#include <string.h>
#include <stdlib.h>

#include "psgplay/assert.h"
#include "psgplay/compare.h"
#include "psgplay/file.h"
#include "psgplay/memory.h"
#include "psgplay/print.h"
#include "psgplay/string.h"
#include "psgplay/tool.h"

#include "sndh/ice.h"
#include "sndh/sndh.h"

#include "unicode/atari.h"

struct tag {
	char *value;
	char buffer[32];

	int subtunes;
	bool hdns;

	bool (*cb)(const char *name, const char *value, void *arg);
	void *arg;

	bool continuation;
};

static size_t sndh_head_offset(struct file file)
{
	char *c = file.data;

	/*
	 * 20 bytes is the smallest possible header with "SNDH" at offset 12
	 * immediately followed by "HDNS".
	 */
	return file.size >= 20 &&
	       c[12] == 'S' &&
	       c[13] == 'N' &&
	       c[14] == 'D' &&
	       c[15] == 'H' ? 16 : 0;
}

static bool sndh_tag(const char *name, size_t *offset,
	size_t bound, struct file file)
{
	const size_t len = strlen(name);
	const char *c = file.data;

	if (*offset + len > bound)
		return false;
	if (strncmp(name, &c[*offset], len) != 0)
		return false;

	*offset += len;

	return true;
}

static void tag_cb(const char *name, const char *value, struct tag *tag)
{
	u8 *v = charset_to_utf8_string((const u8 *)value, strlen(value),
			charset_atari_st_to_utf32, NULL);

	tag->continuation = tag->cb(name, (const char *)v, tag->arg);

	free(v);
}

static bool sndh_nul(const char *name, size_t *offset,
	size_t bound, struct file file)
{
	const char *c = file.data;

	while (*offset < bound)
		if (c[(*offset)++] == '\0')
			return true;

	pr_warn("%s: tag %s missing NUL termination\n", file.path, name);

	return false;
}

static bool sndh_string__(const char *name, struct tag *tag, size_t length,
	size_t *offset, size_t bound, struct file file)
{
	char *c = file.data;

	if (!length) {
		tag->value = &c[*offset];

		if (!sndh_nul(name, offset, bound, file))
			return false;

		return true;
	}

	if (bound < *offset + length + 1)
		return false;

	BUG_ON(sizeof(tag->buffer) < length + 1);

	memcpy(tag->buffer, &c[*offset], length);
	tag->buffer[length] = 0;
	tag->value = tag->buffer;
	*offset += length;

	if (c[*offset] == '\0')
		(*offset)++;
	else
		pr_warn("%s: tag %s missing NUL termination\n",
			file.path, name);

	return true;
}

static bool sndh_string(const char *name, struct tag *tag, size_t length,
	size_t *offset, size_t bound, struct file file)
{
	if (!sndh_string__(name, tag, length, offset, bound, file))
		return false;

	tag_cb(name, tag->value, tag);

	return true;
}

static bool sndh_substrings(const char *name, struct tag *tag, size_t length,
	size_t *offset, size_t bound, struct file file)
{
	const size_t table_start = *offset - strlen(name);

	if (!tag->subtunes) {
		pr_warn("%s: tag %s without any subtunes\n", file.path, name);

		return true;
	}

	if (bound < *offset + tag->subtunes * 2) {
		pr_error("%s: tag %s too short\n", file.path, name);

		return false;
	}

	const u8 *b = file.data;
	const u8 *t = &b[*offset];

	*offset += tag->subtunes * 2;

	for (int i = 0; i < tag->subtunes; i++)
		if (!sndh_nul(name, offset, bound, file))
			return false;

	const size_t table_size = *offset - table_start;

	for (int i = 0; i < tag->subtunes && tag->continuation; i++) {
		const u16 o = (t[2 * i] << 8) | t[2 * i + 1];

		if (o < table_size) {
			tag_cb(name, (const char *)&b[table_start + o], tag);
		} else {
			pr_error("%s: tag %s table too short\n",
				file.path, name);

			return false;
		}
	}


	return true;
}

static bool sndh_integer(const char *name, struct tag *tag, size_t length,
	size_t *offset, size_t bound, struct file file)
{
	if (!sndh_string__(name, tag, length, offset, bound, file))
		return false;

	int n;

	if (!strtoint(&n, tag->value, 10)) {
		pr_warn("%s: tag %s malformed number: %s\n",
			file.path, name, tag->value);

		return false;
	}

	char s[32];
	snprintf(s, sizeof(s), "%d", n);
	tag_cb(name, s, tag);

	return true;
}

static bool sndh_subtunes(const char *name, struct tag *tag, size_t length,
	size_t *offset, size_t bound, struct file file)
{
	if (!sndh_string__(name, tag, length, offset, bound, file))
		return false;

	if (!strtoint(&tag->subtunes, tag->value, 10)) {
		pr_warn("%s: tag %s malformed number\n",
			file.path, name);

		return false;
	}

	char s[32];
	snprintf(s, sizeof(s), "%d", tag->subtunes);
	tag_cb(name, s, tag);

	return true;
}

static bool sndh_time(const char *name, struct tag *tag, size_t length,
	size_t *offset, size_t bound, struct file file)
{
	if (!tag->subtunes) {
		pr_warn("%s: tag %s without any subtunes\n", file.path, name);

		return true;
	}

	if (bound < *offset + tag->subtunes * 2) {
		pr_error("%s: tag %s too short\n", file.path, name);

		return false;
	}

	const u8 *b = file.data;
	for (int i = 0; i < tag->subtunes && tag->continuation; i++) {
		const u8 *t = &b[*offset + i * 2];
		const u16 n = (t[0] << 8) | t[1];
		char s[32];

		snprintf(s, sizeof(s), "%d", n);
		tag_cb(name, s, tag);
	}

	*offset += tag->subtunes * 2;

	return true;
}

static bool sndh_hdns(const char *name, struct tag *tag, size_t length,
	size_t *offset, size_t bound, struct file file)
{
	tag->hdns = true;

	return true;
}

static bool sndh_padding(const char *name, struct tag *tag, size_t length,
	size_t *offset, size_t bound, struct file file)
{
	char *c = file.data;

	if (*offset < file.size && c[*offset] == '\0') {
		(*offset)++;

		return true;
	}

	return false;
}

static void branch_bound(size_t *bound,
	size_t offset, const void *data, size_t size)
{
	const u8 *b = data;

	if (offset + size < 2)
		return;

	const u16 w = (b[offset + 0] << 8) | b[offset + 1];
	if (w == 0x4e71)	/* nop */
		return branch_bound(bound, offset + 2, data, size);

	if ((w & 0xff00) == 0x6000 && (w & 0xff) != 0) {	/* bra.s */
		const size_t a = offset + (w & 0xff);

		if (a < *bound)
			*bound = a;

		return;
	}

	if (offset + size < 4)
		return;

	if (w == 0x6000 ||	/* bra.w */
	    w == 0x4efa) {	/* jmp(pc) */
		const size_t a = offset + 2 +
			((b[offset + 2] << 8) | b[offset + 3]);

		if (a < *bound)
			*bound = a;

		return;
	}
}

static size_t tag_bound(struct file file)
{
	size_t bound = file.size;

	branch_bound(&bound, 0, file.data,  4);
	branch_bound(&bound, 4, file.data,  8);
	branch_bound(&bound, 8, file.data, 12);

	return bound;
}

static bool match_tag(struct tag *tag, size_t *offset,
	size_t bound, struct file file)
{
	static const struct {
		const char *name;
		bool (*read)(const char *name, struct tag *tag, size_t length,
			size_t *offset, size_t bound, struct file file);
		size_t length;
	} tags[] = {
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

	for (size_t i = 0; i < ARRAY_SIZE(tags); i++)
		if (sndh_tag(tags[i].name, offset, bound, file) &&
		    tags[i].read(tags[i].name, tag,
				 tags[i].length, offset,
				 bound, file))
			return true;

	return false;
}

bool sndh_tags(struct file file, size_t *size, sndh_tag_cb cb, void *arg)
{
	size_t offset = sndh_head_offset(file);

	if (size)
		*size = 0;

	if (!offset) {
		pr_error("%s: invalid SNDH header\n", file.path);

		return false;
	}

	const size_t bound = tag_bound(file);
	struct tag tag = {
		.subtunes = 1,	/* Default to 1, unless the ## tag is given */
		.cb = cb,
		.arg = arg,
		.continuation = true
	};

	while (offset < bound && tag.continuation && !tag.hdns)
		if (!match_tag(&tag, &offset, bound, file)) {
			const u8 *b = file.data;

			pr_error("%s: unrecognised data in %zu bytes:\n",
				file.path, bound - offset);
			pr_mem(stderr, &b[offset],
				min(bound - offset, 256ul), offset);
			fprintf(stderr, "\n");

			if (size)
				*size = offset;

			return false;
		}

	if (size)
		*size = offset;

	if (tag.continuation && !tag.hdns)
		pr_warn("%s: missing HDNS tag\n", file.path);

	return tag.continuation;
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
	} else if (option_verbosity())
		pr_warn("%s: not ICE packed\n", file.path);

	return file;
}
