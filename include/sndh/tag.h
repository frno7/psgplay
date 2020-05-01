// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#ifndef PSGPLAY_SNDH_TAG_H
#define PSGPLAY_SNDH_TAG_H

#include <stddef.h>

#include "sndh/timer.h"

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

#endif /* PSGPLAY_SNDH_TAG_H */
