// SPDX-License-Identifier: GPL-2.0

#ifndef PSGPLAY_OUTPUT_ALSA_H
#define PSGPLAY_OUTPUT_ALSA_H

#include "out/output.h"

extern const struct output alsa_output;

/**
 * alsa_output_handle - determine ALSA output handle
 * @output: output string
 *
 * Note: @output must be valid during the lifetime of the returned string.
 *
 * Return: %"default" if @output is %NULL, ALSA handle if @output has
 * 	%"alsa:" prefix, otherwise %NULL.
 */
const char *alsa_output_handle(const char *output);

#endif /* PSGPLAY_OUTPUT_ALSA_H */
