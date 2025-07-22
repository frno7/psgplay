// SPDX-License-Identifier: GPL-2.0

#ifndef TEST_SNDH_H
#define TEST_SNDH_H

#include "internal/macro.h"
#include "internal/struct.h"
#include "internal/types.h"

#define SNDH_TUNE_NAMES_SELECT(identifier, title) &identifier,

#define SNDH_TUNE_SELECT(type, names)					\
	const type _sndh_tune_select_[] = {				\
		names(SNDH_TUNE_NAMES_SELECT)				\
	}

#define sndh_title(title) const char sndh_title[]			\
	__attribute__((section(".sndh.title")))	= "TITL" title

#define SNDH_TUNE_COUNT(d, count) ((((count) / (d)) % 10) + '0')

#define sndh_tune_count(count) const char sndh_tune_count[5]		\
	__attribute__((section(".sndh.tune_count"))) =			\
	{ '#', '#', SNDH_TUNE_COUNT(10, (count)),			\
		    SNDH_TUNE_COUNT( 1, (count)), '\0' }

#define SNDH_TUNE_NAMES_STRUCT_DEFINE_OFFSET(identifier, title)		\
	4 + 2 * ARRAY_SIZE(_sndh_tune_select_) +			\
		offsetof(struct sndh_tune_names_, _title_##identifier),

#define SNDH_TUNE_NAMES_OFFSETS(names)					\
	__attribute__((section(".sndh.tune_name_offsets"))) 		\
	const uint16_t _sndh_tune_name_offsets_[] = {			\
		names(SNDH_TUNE_NAMES_STRUCT_DEFINE_OFFSET)		\
	}

#define SNDH_TUNE_NAMES_STRUCT_DECLARE_ENTRY(identifier, title)		\
	const char _title_##identifier[sizeof(title) + 1];

#define SNDH_TUNE_NAMES_STRUCT_DEFINE_ENTRY(identifier, title)		\
	title,

#define sndh_tune_names(type, names)					\
	SNDH_TUNE_SELECT(type, names);					\
	struct sndh_tune_names_ {					\
		names(SNDH_TUNE_NAMES_STRUCT_DECLARE_ENTRY)		\
	};								\
	__attribute__((section(".sndh.tune_name_tag"))) 		\
	const char _sndh_tune_name_tag_[4] = "!#SN";			\
	SNDH_TUNE_NAMES_OFFSETS(names);					\
	__attribute__((section(".sndh.tune_names")))			\
	struct sndh_tune_names_ _sndh_tune_names_ = {			\
		names(SNDH_TUNE_NAMES_STRUCT_DEFINE_ENTRY)		\
	};								\
	sndh_tune_count(ARRAY_SIZE(_sndh_tune_name_offsets_));

#define sndh_tune_select(tune) (_sndh_tune_select_[(tune) - 1])

void sndh_init(int tune);
void sndh_exit();
void sndh_play();

#endif /* TEST_SNDH_H */
