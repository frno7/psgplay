// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#ifndef PSGPLAY_SNDH_H
#define PSGPLAY_SNDH_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define SNDH_PROLOGUE_SIZE 20

#define SNDH_FLAG(f)							\
	f('y', PSG, "YM2149 PSG")					\
	f('e', DMA, "STE/TT DMA sound")					\
	f('a', MFP_TA, "MFP timer A")					\
	f('b', MFP_TB, "MFP timer B")					\
	f('c', MFP_TC, "MFP timer C")					\
	f('d', MFP_TD, "MFP timer D")					\
	f('p', AGA, "Amiga AGA")					\
	f('l', LMC, "STE/TT LMC1992")					\
	f('s', DSP, "Falcon DSP 56001")					\
	f('t', BLT, "BLITTER")						\
	f('h', HBL, "Horizontal blank")

enum sndh_timer_type {
	SNDH_TIMER_A = 'A',
	SNDH_TIMER_B = 'B',
	SNDH_TIMER_C = 'C',
	SNDH_TIMER_D = 'D',
	SNDH_TIMER_V = 'V',	/* Vertical blank (VBL) */
};

struct sndh_timer {
	enum sndh_timer_type type;
	int frequency;
};

/**
 * sndh_identify - is data SNDH?
 * @data: data
 * @size: size in bytes of data, or greater
 *
 * Note that @size must be at least SNDH_PROLOGUE_SIZE bytes.
 *
 * Return: %true if data seems to be SNDH, otherwise %false
 */
bool sndh_identify(const void *data, size_t size);

/**
 * sndh_tag_subtune_count - get SNDH subtune count
 * @subtune_count: SNDH subtune count result, if determined
 * @data: SNDH data
 * @size: size in bytes of SNDH data
 *
 * Return: %true on success, otherwise %false
 */
bool sndh_tag_subtune_count(int *subtune_count,
	const void *data, const size_t size);

/**
 * sndh_tag_default_subtune - get SNDH default subtune
 * @default_subtune: default SNDH subtune result, if determined
 * @data: SNDH data
 * @size: size in bytes of SNDH data
 *
 * Return: %true on success, otherwise %false
 */
bool sndh_tag_default_subtune(int *default_subtune,
	const void *data, const size_t size);

/**
 * sndh_tag_subtune_time - get SNDH subtune duration time
 * @duration: result of SNDH subtune duration in seconds, if determined
 * @data: SNDH data
 * @size: size in bytes of SNDH data
 *
 * Return: %true on success, otherwise %false
 */
bool sndh_tag_subtune_time(float *duration, int subtune,
	const void *data, const size_t size);

/**
 * sndh_tag_timer - get SNDH timer
 * @timer: SNDH timer result, if determined
 * @data: SNDH data
 * @size: size in bytes of SNDH data
 *
 * Return: %true on success, otherwise %false
 */
bool sndh_tag_timer(struct sndh_timer *timer,
	const void *data, const size_t size);

/**
 * sndh_tag_title - get SNDH title
 * @title: SNDH title result, if determined
 * @length: maximum length of title buffer including NUL
 * @data: SNDH data
 * @size: size in bytes of SNDH data
 *
 * Return: %true on success, otherwise %false
 */
bool sndh_tag_title(char *title, size_t length,
	const void *data, const size_t size);

/**
 * sndh_init_address - SNDH init address relative start of data
 * @data: SNDH data
 * @size: size in bytes of SNDH data
 *
 * Return: address relative start of data, otherwise zero
 */
size_t sndh_init_address(const void *data, const size_t size);

/**
 * sndh_exit_address - SNDH exit address relative start of data
 * @data: SNDH data
 * @size: size in bytes of SNDH data
 *
 * Return: address relative start of data, otherwise zero
 */
size_t sndh_exit_address(const void *data, const size_t size);

/**
 * sndh_play_address - SNDH play address relative start of data
 * @data: SNDH data
 * @size: size in bytes of SNDH data
 *
 * Return: address relative start of data, otherwise zero
 */
size_t sndh_play_address(const void *data, const size_t size);

/** sndh_tag_name - SNDH tag name */
#define sndh_tag_name (sndh_cursor__.tag->name)

/** sndh_tag_value - SNDH tag string representation */
#define sndh_tag_value (sndh_cursor__.value)

/** sndh_tag_integer - SNDH tag integer representation */
#define sndh_tag_integer (sndh_cursor__.integer)

/**
 * sndh_for_each_tag - iterate over SNDH tags
 * @data: SNDH data
 * @size: size in bytes of SNDH data
 *
 * Note that some tags such as "TIME" and "!#SN" may have multiple values
 * and be given more than once.
 */
#define sndh_for_each_tag(data, size)					\
	sndh_for_each_tag_with_header_size_and_diagnostic((data), (size), NULL, NULL)

/**
 * sndh_for_each_tag_with_diagnostic - iterate over SNDH tags with diagnostic
 * @data: SNDH data
 * @size: size in bytes of SNDH data
 * @diag: pointer to &struct sndh_diagnostic with warning and error callbacks,
 * 	or %NULL to ignore
 *
 * Note that some tags such as "TIME" and "!#SN" may have multiple values
 * and be given more than once.
 */
#define sndh_for_each_tag_with_diagnostic(data, size, diag)		\
	sndh_for_each_tag_with_header_size_and_diagnostic((data), (size), NULL, (diag))

/**
 * sndh_for_each_tag_with_header_size - iterate over SNDH tags and compute
 * 	SNDH header size
 * @data: SNDH data
 * @size: size in bytes of SNDH data
 * @header_size: pointer to &size_t to store SNDH header size, or %NULL to
 * 	ignore
 *
 * Note that some tags such as "TIME" and "!#SN" may have multiple values
 * and be given more than once.
 */
#define sndh_for_each_tag_with_header_size(data, size, header_size)	\
	sndh_for_each_tag_with_header_size_and_diagnostic((data), (size), (header_size), NULL)

/**
 * sndh_for_each_tag_with_header_size_and_diagnostic - iterate over SNDH tags
 * 	and compute size with diagnostic
 * @data: SNDH data
 * @size: size in bytes of SNDH data
 * @header_size: pointer to &size_t to store SNDH header size, or %NULL to
 * 	ignore
 * @diag: pointer to &struct sndh_diagnostic with warning and error callbacks,
 * 	or %NULL to ignore
 *
 * Note that some tags such as "TIME" and "!#SN" may have multiple values
 * and be given more than once.
 */
#define sndh_for_each_tag_with_header_size_and_diagnostic(data, size, header_size, diag)\
	for (struct sndh_cursor sndh_cursor__ =				\
	     sndh_first_tag((data), (size), (header_size), (diag));	\
	     sndh_next_tag(&sndh_cursor__), sndh_valid_tag(&sndh_cursor__);)

/**
 * struct sndh_diagnostic - SNDH tag diagnostic message callbacks
 * @warn: diagnostic warning message
 * @error: diagnostic error message
 * @arg: optional callback argument, can be %NULL
 */
struct sndh_diagnostic {
	void (*warn)(void *arg, const char *fmt, ...)
		__attribute__((format(printf, 2, 3)));
	void (*error)(void *arg, const char *fmt, ...)
		__attribute__((format(printf, 2, 3)));
	void *arg;
};

/* Private SNDH structures and functions. */

struct sndh_cursor;

struct sndh_tag {
	const char *name;
	bool (*read)(struct sndh_cursor *cursor);
	size_t length;
};

struct sndh_cursor {
	const struct {
		size_t size;
		const void *data;
	} file;

	struct {
		size_t *size;
	} header;

	const size_t bound;
	size_t offset;

	bool valid;

	const struct sndh_tag *tag;

	int integer;
	const char *value;
	char buffer[32];

	int subtunes;

	struct {
		bool (*read)(struct sndh_cursor *cursor);
		size_t start;
		size_t bound;
	} subtag;

	bool hdns;

	const struct sndh_diagnostic diag;
};

struct sndh_cursor sndh_first_tag(const void *data, const size_t size,
	size_t *header_size, const struct sndh_diagnostic *diag);

size_t sndh_header_size(const void *data, const size_t size);

bool sndh_valid_tag(const struct sndh_cursor *cursor);

void sndh_next_tag(struct sndh_cursor *cursor);

struct sndh_file {
	size_t size;
	struct {
		uint32_t init;
		uint32_t exit;
		uint32_t play;
		uint8_t data[];
	} *sndh;
};

static inline uint32_t sndh_timer_to_u32(const struct sndh_timer timer)
{
	return (timer.frequency << 8) | (timer.type & 0xff);
}

static inline struct sndh_timer u32_to_sndh_timer(const uint32_t timer)
{
	return (struct sndh_timer) {
		.type = timer & 0xff,
		.frequency = timer >> 8,
	};
}

#if defined(__m68k__)

static inline void sndh_init(int track, struct sndh_file *file)
{
	__asm__ __volatile__ (
		"	movem.l	%%d0-%%a6,-(%%sp)\n"
		"	move.l	%1,%%d0\n"
		"	jsr	(%0)\n"
		"	movem.l	(%%sp)+,%%d0-%%a6\n"
		:
		: "a" (&file->sndh->init), "d" (track)
		: "memory");
}

static inline void sndh_exit(struct sndh_file *file)
{
	__asm__ __volatile__ (
		"	movem.l	%%d0-%%a6,-(%%sp)\n"
		"	jsr	(%0)\n"
		"	movem.l	(%%sp)+,%%d0-%%a6\n"
		:
		: "a" (&file->sndh->exit)
		: "memory");
}

static inline void sndh_play(struct sndh_file *file)
{
	__asm__ __volatile__ (
		/* Enable interrupts and other timers for effects. */
		"	move	#0x2200,%%sr\n"
		"	movem.l	%%d0-%%a6,-(%%sp)\n"
		"	jsr	(%0)\n"
		"	movem.l	(%%sp)+,%%d0-%%a6\n"
		:
		: "a" (&file->sndh->play)
		: "memory");
}

#endif /* defined(__m68k__) */

#endif /* PSGPLAY_SNDH_H */
