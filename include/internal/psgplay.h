// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2026 Fredrik Noring
 */

#ifndef INTERNAL_PSGPLAY_H
#define INTERNAL_PSGPLAY_H

#include <stdint.h>
#include <stddef.h>

#include "atari/dac.h"
#include "atari/machine.h"

#include "psgplay/stereo.h"

struct fir8 {
	int16_t xn[8];
	int k;
};

struct stereo_buffer {
	size_t index;
	size_t count;
	size_t capacity;
	size_t total;
	struct psgplay_stereo *sample;
};

struct digital_buffer {
	size_t index;
	struct {
		size_t psg;
		size_t sound;
		size_t mixer;
	} count;
	size_t capacity;
	size_t total;
	size_t stop;
	struct psgplay_digital *sample;
};

struct psgplay {
	struct stereo_buffer stereo_buffer;
	struct digital_buffer digital_buffer;

	struct {
		uint64_t psg;
		uint64_t dma;
		uint64_t mix;
		uint64_t play;
	} record;

	struct psgplay_downsample {
		int stereo_frequency;
		uint64_t psg_cycle;
		uint64_t downsample_sample_cycle;

		struct {
			struct fir8 left;
			struct fir8 right;
		} lowpass;
	} downsample;

	struct cf2149_dac dac;

	struct {
		psgplay_digital_to_stereo_cb cb;
		void *arg;
	} digital_to_stereo_callback;

	struct {
		psgplay_stereo_downsample_cb cb;
		void *arg;
	} stereo_downsample_callback;

	struct machine machine;

	struct {
		void (*cb)(uint32_t pc, void *arg);
		void *arg;
	} instruction_callback;

	int errno_;
};

#endif /* INTERNAL_PSGPLAY_H */
