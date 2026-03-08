// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "internal/print.h"

#include "psgplay/sndh.h"

#include "system/unix/diagnostic.h"
#include "system/unix/file.h"
#include "system/unix/string.h"

static void diag_warn(void *arg, const char *fmt, ...)
{
	struct file *file = arg;
	char msg[1024];
	va_list ap;

	va_start(ap, fmt);
	vsnprintf(msg, sizeof(msg), fmt, ap);
	pr_warn("%s: %s\n", file->path, msg);
	va_end(ap);
}

static void diag_error(void *arg, const char *fmt, ...)
{
	struct file *file = arg;
	char msg[1024];
	va_list ap;

	va_start(ap, fmt);
	vsnprintf(msg, sizeof(msg), fmt, ap);
	pr_error("%s: %s\n", file->path, msg);
	va_end(ap);
}

static bool printable(const char *s)
{
	for (size_t i = 0; s[i] != '\0'; i++) {
		int c = s[i] & 0xff;

		if (c < 0x20 || 0x7f == c)
			return false;
	}

	return true;
}

void sndh_diagnostic(struct file file)
{
	const struct sndh_diagnostic diag = {
		.warn = diag_warn,
		.error = diag_error,
		.arg = &file
	};

	sndh_for_each_tag_with_diagnostic (file.data, file.size, &diag) {
		const char *name = sndh_tag_name;
		const char *text = sndh_tag_text;
		const size_t length = strlen(text);

		if (!length)
			pr_warn("%s: tag %s empty\n", file.path, name);
		if (!printable(text))
			pr_warn("%s: tag %s contains nonprintable characters: %s\n",
				file.path, name, text);
		if (length && (isspace(text[0]) || isspace(text[length - 1])))
			pr_warn("%s: tag %s malformed whitespace\n",
				file.path, name);

	}
}
