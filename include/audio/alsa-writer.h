// SPDX-License-Identifier: GPL-2.0

#ifndef PSGPLAY_ALSA_WRITER_H
#define PSGPLAY_ALSA_WRITER_H

#include "audio/writer.h"

extern const struct audio_writer alsa_writer;

/**
 * alsa_writer_handle - determine ALSA output handle
 * @output: output string
 *
 * Note: @output must be valid during the lifetime of the returned string.
 *
 * Return: %"default" if @output is %NULL, ALSA handle if @output has
 * 	%"alsa:" prefix, otherwise %NULL.
 */
const char *alsa_writer_handle(const char *output);

#endif /* PSGPLAY_ALSA_WRITER_H */
