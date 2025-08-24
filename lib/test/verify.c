// SPDX-License-Identifier: GPL-2.0

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "internal/print.h"

#include "system/unix/file.h"
#include "system/unix/memory.h"

#include "audio/audio.h"
#include "graph/graph.h"
#include "graph/svg.h"

#include "test/option.h"
#include "test/report.h"

const char *progname;

static bool encode_file(const void *data, size_t size, void *arg)
{
	int fd = *(int *)arg;

	if (xwrite(fd, data, size) != size)
		pr_fatal_errno("Failed to write\n");

	return true;
}

static void graph(const struct options *options,
	const struct audio *audio)
{
	struct audio *cut = audio_range(audio, 0, 250);
	struct audio *norm = audio_normalise(cut, 0.8f);
	const struct audio_meter meter = audio_meter(norm);
	const struct audio_zero_crossing_periodic zcp =
		audio_zero_crossing_periodic(norm);
	const struct audio_wave wave = audio_wave_estimate(zcp);

	int fd = xopen(options->output, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd == -1)
		pr_fatal_errno(options->output);

	struct graph_encoder *encoder = graph_encoder_init(
		(struct graph_bounds) {
			.min_x =    0,
			.min_y =    0,
			.max_x = 1000,
			.max_y =  200,
		},
		(struct graph_encoder_cb) {
			.f = encode_file,
			.arg = &fd,
		},
		&svg_encoder);

	encoder->module->header(encoder);
	encoder->module->axes(encoder);
	encoder->module->square_wave(encoder,
		wave, meter.left.minimum, meter.left.maximum);
	encoder->module->samples(encoder, norm);
	encoder->module->footer(encoder);

	graph_encoder_free(encoder);

	if (xclose(fd) == -1)
		pr_fatal_errno(options->output);

	audio_free(norm);
	audio_free(cut);
}

int main(int argc, char *argv[])
{
	struct options *options = parse_options(argc, argv);

	struct audio *audio = audio_read_wave(options->input);

	/* FIXME: Avoid trimming the last second with --no-fade option. */
	if (audio->format.sample_count < 2 * audio->format.frequency)
		pr_fatal_error("%s: too short: must be at least %d samples\n",
			options->input, 2 * audio->format.frequency);
	struct audio *trim = audio_range(audio, 0,
		audio->format.sample_count - audio->format.frequency);

	if (strcmp(options->command, "graph") == 0)
		graph(options, trim);
	else if (strcmp(options->command, "report") == 0)
		report(trim, options);
	else
		pr_fatal_error("%s: unknown command\n", options->command);

	audio_free(trim);
	audio_free(audio);

	return EXIT_SUCCESS;
}
