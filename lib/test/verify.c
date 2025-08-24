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
	struct strbuf *sb = arg;

	sbprintf(sb, "%*s", (int)size, data);

	return true;
}

static void graph(struct strbuf *sb, const struct audio *audio,
	const struct options *options)
{
	struct audio *cut = audio_range(audio, 0, 250);
	struct audio *norm = audio_normalise(cut, 0.8f);
	const struct audio_meter meter = audio_meter(norm);
	const struct audio_zero_crossing_periodic zcp =
		audio_zero_crossing_periodic(norm);
	const struct audio_wave wave = audio_wave_estimate(zcp);

	struct graph_encoder *encoder = graph_encoder_init(
		(struct graph_bounds) {
			.min_x =    0,
			.min_y =    0,
			.max_x = 1000,
			.max_y =  200,
		},
		(struct graph_encoder_cb) {
			.f = encode_file,
			.arg = sb,
		},
		&svg_encoder);

	encoder->module->header(encoder);
	encoder->module->axes(encoder);
	encoder->module->square_wave(encoder,
		wave, meter.left.minimum, meter.left.maximum);
	encoder->module->samples(encoder, norm);
	encoder->module->footer(encoder);

	graph_encoder_free(encoder);

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

	struct strbuf sb = { };

	if (strcmp(options->command, "graph") == 0) {
		graph(&sb, trim, options);
	} else if (strcmp(options->command, "report") == 0) {
		report(&sb, trim, options);
	} else
		pr_fatal_error("%s: unknown command\n", options->command);

	if (options->output && sb.length &&
	    !file_write(options->output, sb.s, sb.length))
		pr_fatal_errno(options->output);

	sbfree(&sb);

	audio_free(trim);
	audio_free(audio);

	return EXIT_SUCCESS;
}
