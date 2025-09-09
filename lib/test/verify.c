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
#include "test/verify.h"

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
	struct audio *norm = audio_normalise(audio, 0.8f);
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

	const size_t margin = 10;
	const size_t i = zcp.first.index >= margin ?
		zcp.first.index - margin : 0;
	struct audio *cut = audio_range(norm, i, i + 250);

	encoder->module->header(encoder);
	encoder->module->axes(encoder);
	encoder->module->square_wave(encoder,
		wave, meter.left.minimum, meter.left.maximum);
	encoder->module->samples(encoder, cut);
	encoder->module->footer(encoder);

	audio_free(cut);

	graph_encoder_free(encoder);

	audio_free(norm);
}

__attribute__((weak)) const char *flags(const struct options *options)
{
	return "";
}

int main(int argc, char *argv[])
{
	struct options *options = parse_options(argc, argv);

	if (strcmp(options->command, "flags") == 0) {
		puts(flags(options));

		return EXIT_SUCCESS;
	}

	if (!options->input)
		pr_fatal_error("missing input WAVE file\n");

	name_from_input(options);

	if (!options->track)
		options->track = track_from_path(options->input);
	if (!options->track)
		pr_fatal_error("%s: track not in file name and not given with --track\n",
			options->input);

	struct audio *audio = audio_read_wave(options->input);

	/* FIXME: Avoid trimming first and last second with --no-fade option. */
	if (audio->format.sample_count < 3 * audio->format.frequency)
		pr_fatal_error("%s: too short: must be at least %d samples\n",
			options->input, 2 * audio->format.frequency);
	struct audio *trim = audio_range(audio,
		audio->format.frequency,
		audio->format.sample_count - audio->format.frequency);

	struct strbuf sb = { };

	if (strcmp(options->command, "verify") == 0) {
		const char *error = verify(trim, options);

		if (error) {
			report(&sb, trim, options);

			pr_fatal_error("%s\n%s%s: error: verify: %s\n",
				options->input, sb.s, options->input, error);
		}
	} else if (strcmp(options->command, "graph") == 0) {
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
