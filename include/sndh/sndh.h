// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#ifndef PSGPLAY_SNDH_H
#define PSGPLAY_SNDH_H

#include "psgplay/types.h"

#define SNDH_FLAG(f)							\
	f('y', PSG)							\
	f('e', DMA)							\
	f('a', MFP_TA)							\
	f('b', MFP_TB)							\
	f('c', MFP_TC)							\
	f('d', MFP_TD)							\
	f('p', AGA)							\
	f('l', LMC)							\
	f('s', DSP)							\
	f('t', BLT)							\
	f('h', HBL)

/**
 * sndh_tag_cb - SNDH tag callback
 * @name: tag name
 * @value: tag value
 * @arg: optional pointer supplied to &sndh_tags;
 *
 * Note that some tags such as "TIME" and "#!SN" may have multiple values and
 * this callback will be invoked, in order, for each value separately.
 *
 * Return: %true to continue processing next tag, otherwise %false
 */
typedef bool (*sndh_tag_cb)(const char *name, const char *value, void *arg);

/**
 * sndh_tags - iterate over all SNDH tags
 * @file: file with SNDH data
 * @size: store total tag size, unless %NULL
 * @cb: callback to invoke for each tag
 * @arg: optional pointer passed on to the callback; can be %NULL
 *
 * Return: %true on successful completion, otherwise %false
 */
bool sndh_tags(struct file file, size_t *size, sndh_tag_cb cb, void *arg);

#endif /* PSGPLAY_SNDH_H */
