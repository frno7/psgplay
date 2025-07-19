// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "internal/assert.h"
#include "internal/compare.h"

#include "atari/cpu.h"
#include "atari/machine.h"
#include "atari/psg.h"

#include "psgplay/psgplay.h"
#include "psgplay/sndh.h"

#include "cf2149/module/cf2149.h"
#include "cf2149/module/dac.h"

#define FADE_SAMPLES 2500	/* 10 ms with 250 kHz */

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

	struct psgplay_downsample {
		int stereo_frequency;
		u64 psg_cycle;
		u64 downsample_sample_cycle;

		struct {
			struct fir8 left;
			struct fir8 right;
		} lowpass;
	} downsample;

	struct {
		psgplay_digital_to_stereo_cb cb;
		void *arg;
	} digital_to_stereo_callback;

	struct {
		psgplay_stereo_downsample_cb cb;
		void *arg;
	} stereo_downsample_callback;
	
	uint16_t dac[32][32][32];
	const struct machine *machine;

	struct {
		void (*cb)(uint32_t pc, void *arg);
		void *arg;
	} instruction_callback;

	int errno_;
};

static int buffer_stereo_sample(struct stereo_buffer *sb,
	const struct psgplay_stereo *stereo, const size_t count)
{
	for (size_t i = 0; i < count; i++) {
		if (sb->capacity <= sb->count) {
			const size_t capacity = sb->capacity +
				max_t(size_t, sb->capacity, 1024);

			void *sample = realloc(sb->sample,
				capacity * sizeof(*sb->sample));
			if (!sample)
				return errno;

			sb->sample = sample;
			sb->capacity = capacity;
		}

		sb->sample[sb->count++] = stereo[i];
	}

	return 0;
}

static int digital_buffer_allocate(struct digital_buffer *db)
{
	const size_t capacity = db->capacity +
		max_t(size_t, db->capacity, 1024);

	void *sample = realloc(db->sample, capacity * sizeof(*db->sample));
	if (!sample)
		return errno;

	db->sample = sample;
	db->capacity = capacity;

	return 0;
}

static int buffer_digital_psg_sample(const struct cf2149_ac *sample,
	struct digital_buffer *db)
{
	int err = 0;

	if (db->capacity <= db->count.psg)
		err = digital_buffer_allocate(db);

	if (!err)
		db->sample[db->count.psg++].psg =
			(struct psgplay_digital_psg) {
				.lva.u8 = sample->lva.u8,
				.lvb.u8 = sample->lvb.u8,
				.lvc.u8 = sample->lvc.u8,
			};

	return err;
}

static int buffer_digital_sound_sample(const struct sound_sample *sample,
	struct digital_buffer *db)
{
	int err = 0;

	if (db->capacity <= db->count.sound)
		err = digital_buffer_allocate(db);

	if (!err)
		db->sample[db->count.sound++].sound =
			(struct psgplay_digital_sound) {
				.left  = sample->left,
				.right = sample->right,
			};

	return err;
}

static int buffer_digital_mixer_sample(const struct mixer_sample *sample,
	struct digital_buffer *db)
{
	int err = 0;

	if (db->capacity <= db->count.mixer)
		err = digital_buffer_allocate(db);

	if (!err)
		db->sample[db->count.mixer++].mixer =
			(struct psgplay_digital_mixer) {
				.volume = {
					.main = sample->volume.main,
					.left = sample->volume.left,
					.right = sample->volume.right,
				},
				.tone = {
					.bass = sample->tone.bass,
					.treble = sample->tone.treble,
				},
				.mix = sample->mix,
			};

	return err;
}

static int16_t sample_lowpass(int16_t sample, struct fir8 *lowpass)
{
	lowpass->xn[lowpass->k++ % ARRAY_SIZE(lowpass->xn)] = sample;

	s32 x = 0;
	for (int i = 0; i < ARRAY_SIZE(lowpass->xn); i++)
		x += lowpass->xn[i];	/* Simplistic 8 tap FIR filter. */

	return x / ARRAY_SIZE(lowpass->xn);
}

struct mixer {
	bool enable;
	struct {
		int left;
		int right;
	} volume;
	struct {
		float left;
		float right;
	} gain;
};

static struct mixer mixer_init(
	const struct psgplay_digital *digital, size_t count)
{
	int8_t enable = 0;

	for (size_t i = 0; i < count; i++)
		enable |= digital[i].mixer.volume.main
		       |  digital[i].mixer.volume.left
		       |  digital[i].mixer.volume.right
		       |  digital[i].mixer.tone.bass
		       |  digital[i].mixer.tone.treble;

	return (struct mixer) {
		.enable = enable,
		.gain = { .left = 1.0f, .right = 1.0 }
	};
}

static float gain_from_volume(const int volume)
{
	static const float gain[] = {
		1.000000000, /* g = 10^(v/20) for 0 ... -120 dB */
		0.891250938, 0.794328235, 0.707945784, 0.630957344,
		0.562341325, 0.501187234, 0.446683592, 0.398107171,
		0.354813389, 0.316227766, 0.281838293, 0.251188643,
		0.223872114, 0.199526231, 0.177827941, 0.158489319,
		0.141253754, 0.125892541, 0.112201845, 0.100000000,
		0.089125094, 0.079432823, 0.070794578, 0.063095734,
		0.056234133, 0.050118723, 0.044668359, 0.039810717,
		0.035481339, 0.031622777, 0.028183829, 0.025118864,
		0.022387211, 0.019952623, 0.017782794, 0.015848932,
		0.014125375, 0.012589254, 0.011220185, 0.010000000,
		0.008912509, 0.007943282, 0.007079458, 0.006309573,
		0.005623413, 0.005011872, 0.004466836, 0.003981072,
		0.003548134, 0.003162278, 0.002818383, 0.002511886,
		0.002238721, 0.001995262, 0.001778279, 0.001584893,
		0.001412538, 0.001258925, 0.001122018, 0.001000000,
		0.000891251, 0.000794328, 0.000707946, 0.000630957,
		0.000562341, 0.000501187, 0.000446684, 0.000398107,
		0.000354813, 0.000316228, 0.000281838, 0.000251189,
		0.000223872, 0.000199526, 0.000177828, 0.000158489,
		0.000141254, 0.000125893, 0.000112202, 0.000100000,
		0.000089125, 0.000079433, 0.000070795, 0.000063096,
		0.000056234, 0.000050119, 0.000044668, 0.000039811,
		0.000035481, 0.000031623, 0.000028184, 0.000025119,
		0.000022387, 0.000019953, 0.000017783, 0.000015849,
		0.000014125, 0.000012589, 0.000011220, 0.000010000,
		0.000008913, 0.000007943, 0.000007079, 0.000006310,
		0.000005623, 0.000005012, 0.000004467, 0.000003981,
		0.000003548, 0.000003162, 0.000002818, 0.000002512,
		0.000002239, 0.000001995, 0.000001778, 0.000001585,
		0.000001413, 0.000001259, 0.000001122, 0.000001000,
	};

	return gain[clamp_t(int, -volume, 0, ARRAY_SIZE(gain) - 1)];
}

static inline void mixer_for_sample(struct mixer *m,
	const struct psgplay_digital d)
{
	if (m->volume.left  == d.mixer.volume.main + d.mixer.volume.left &&
	    m->volume.right == d.mixer.volume.main + d.mixer.volume.right)
		return;

	m->volume.left  = d.mixer.volume.main + d.mixer.volume.left;
	m->volume.right = d.mixer.volume.main + d.mixer.volume.right;

	m->gain.left  = gain_from_volume(m->volume.left);
	m->gain.right = gain_from_volume(m->volume.right);
}

static inline struct psgplay_stereo stereo_mix(struct mixer *m,
	const int16_t sl, const int16_t sr, const struct psgplay_digital d)
{
	const float psg_mix = 0.65;
	const int p = 256 * psg_mix;
	const int q = 256 - p;

	if (m->enable) {
		mixer_for_sample(m, d);

		return (struct psgplay_stereo) {
			.left  = m->gain.left  * (q*d.sound.left  + p*sl) / 256,
			.right = m->gain.right * (q*d.sound.right + p*sr) / 256
		};
	} else
		return (struct psgplay_stereo) {
			.left  = (q*d.sound.left  + p*sl) / 256,
			.right = (q*d.sound.right + p*sr) / 256
		};
}

static int16_t psg_dac(const union psgplay_digital_level level)
{
#define DAC_S16_BITS(S) ((S) * 0xffff - 0x8000)
	static const int16_t dac[32] = CF2149_DAC_5_BIT_LEVEL(DAC_S16_BITS);

	return dac[level.u5];
}

void psgplay_digital_to_stereo_linear(struct psgplay *pp, struct psgplay_stereo *stereo,
	const struct psgplay_digital *digital, size_t count, void *arg)
{
	struct mixer m = mixer_init(digital, count);

	for (size_t i = 0; i < count; i++) {
		const int16_t sa = psg_dac(digital[i].psg.lva);
		const int16_t sb = psg_dac(digital[i].psg.lvb);
		const int16_t sc = psg_dac(digital[i].psg.lvc);

		/* Simplistic linear channel mix. */
		const int16_t s = digital->mixer.mix ? (sa + sb + sc) / 3 : 0;

		stereo[i] = stereo_mix(&m, s, s, digital[i]);
	}
}

void psgplay_digital_to_stereo_balance(struct psgplay *pp, struct psgplay_stereo *stereo,
	const struct psgplay_digital *digital, size_t count, void *arg)
{
	struct psgplay_psg_stereo_balance *w = arg;
	struct mixer m = mixer_init(digital, count);

#define BALANCE(ch, op) (int)(clamp(256.f * (1.f op w->ch), 0.f, 256.f) + 0.5f)
	const int la = BALANCE(a, -), ra = BALANCE(a, +);
	const int lb = BALANCE(b, -), rb = BALANCE(b, +);
	const int lc = BALANCE(c, -), rc = BALANCE(c, +);
#undef BALANCE

	for (size_t i = 0; i < count; i++) {
		const int16_t sa = psg_dac(digital[i].psg.lva);
		const int16_t sb = psg_dac(digital[i].psg.lvb);
		const int16_t sc = psg_dac(digital[i].psg.lvc);

		if (digital->mixer.mix) {
			const int16_t sl = (la*sa + lb*sb + lc*sc) / (256 * 3);
			const int16_t sr = (ra*sa + rb*sb + rc*sc) / (256 * 3);

			stereo[i] = stereo_mix(&m, sl, sr, digital[i]);
		} else
			stereo[i] = stereo_mix(&m, 0, 0, digital[i]);
	}
}

void psgplay_digital_to_stereo_volume(struct psgplay *pp, struct psgplay_stereo *stereo,
	const struct psgplay_digital *digital, size_t count, void *arg)
{
	struct psgplay_psg_stereo_volume *w = arg;
	struct mixer m = mixer_init(digital, count);

#define VOLUME(ch) (int)clamp(256.f * w->ch, 0.f, 256.f)
	const int va = VOLUME(a);
	const int vb = VOLUME(b);
	const int vc = VOLUME(c);
#undef VOLUME

	for (size_t i = 0; i < count; i++) {
		const int16_t sa = psg_dac(digital[i].psg.lva);
		const int16_t sb = psg_dac(digital[i].psg.lvb);
		const int16_t sc = psg_dac(digital[i].psg.lvc);

		const int16_t s = digital->mixer.mix ?
			(va*sa + vb*sb + vc*sc) / (256 * 3) : 0;

		stereo[i] = stereo_mix(&m, s, s, digital[i]);
	}
}

void psgplay_digital_to_stereo_empiric(struct psgplay *pp, struct psgplay_stereo *stereo,
	const struct psgplay_digital *digital, size_t count, void *arg)
{
	struct mixer m = mixer_init(digital, count);
	
	for (size_t i = 0; i < count; i++) {
		const int16_t s = digital->mixer.mix ?
			pp->dac[digital[i].psg.lvc.u5]
			   [digital[i].psg.lvb.u5]
			   [digital[i].psg.lva.u5] - 0x8000 : 0;

		stereo[i] = stereo_mix(&m, s, s, digital[i]);
	}
}

void psgplay_digital_to_stereo_callback(struct psgplay *pp,
	const psgplay_digital_to_stereo_cb cb, void *arg)
{
	pp->digital_to_stereo_callback.cb = cb;
	pp->digital_to_stereo_callback.arg = arg;
}

static size_t stereo_downsample(struct psgplay_stereo *resample,
	const struct psgplay_stereo *stereo, size_t count, void *arg)
{
	struct psgplay_downsample *ds = arg;
	size_t r = 0;

	for (size_t i = 0; i < count; i++) {
		const u64 n = (ds->stereo_frequency * ds->psg_cycle) / PSG_FREQUENCY;
		const struct psgplay_stereo s = {
			.left  = sample_lowpass(stereo[i].left,  &ds->lowpass.left),
			.right = sample_lowpass(stereo[i].right, &ds->lowpass.right)
		};

		for (; ds->downsample_sample_cycle < n; ds->downsample_sample_cycle++)
			resample[r++] = s;

		ds->psg_cycle += 8;
	}

	return r;
}

void psgplay_stereo_downsample_callback(struct psgplay *pp,
	const psgplay_stereo_downsample_cb cb, void *arg)
{
	pp->stereo_downsample_callback.cb = cb;
	pp->stereo_downsample_callback.arg = arg;
}

static float fade(const float x)
{
	static const float gain[] = {
		/*
		 * logistic(x) = 1 / (1 + 2.7182818^-x)
		 * gain(x) = logistic(12*x - 6)
		 */
		0.002472623, 0.003637179, 0.005347276, 0.007855073,
		0.011525363, 0.016881421, 0.024664433, 0.035904679,
		0.051994332, 0.074735184, 0.106306900, 0.149067261,
		0.205080430, 0.275330813, 0.358784157, 0.451763392,
		0.548236608, 0.641215843, 0.724669187, 0.794919570,
		0.850932739, 0.893693100, 0.925264816, 0.948005668,
		0.964095321, 0.975335567, 0.983118579, 0.988474637,
		0.992144927, 0.994652724, 0.996362821, 0.997527377, 1.0
	};

	const float c = clamp(x, 0.0f, 1.0f) * (ARRAY_SIZE(gain) - 2);
	const int i = c;
	const float t = c - i;

	return gain[i] * (1.0f - t) + gain[i + 1] * t;
}

static void stereo_fade_in(struct psgplay_stereo *stereo,
	const size_t count, const ssize_t offset)
{
	const ssize_t n = FADE_SAMPLES;

	for (ssize_t i = 0; i < count && offset + i < n; i++) {
		const size_t k = offset + i;
		const float g = fade((float)k / (float)n);

		stereo[i].left  = g * stereo[i].left;
		stereo[i].right = g * stereo[i].right;
	}
}

static void stereo_fade_out(struct psgplay_stereo *stereo,
	const size_t count, const ssize_t offset)
{
	const ssize_t n = FADE_SAMPLES;

	for (ssize_t i = 0; i < count; i++) {
		const ssize_t k = offset + i;
		const float g = 1.0f - fade((float)k / (float)n);

		stereo[i].left  = g * stereo[i].left;
		stereo[i].right = g * stereo[i].right;
	}
}

static void stereo_fade(struct psgplay_stereo *stereo,
	const ssize_t count, const ssize_t offset, const ssize_t stop)
{
	stereo_fade_in(stereo, count, offset);

	if (stop && offset + count + FADE_SAMPLES >= stop)
		stereo_fade_out(stereo, count, offset - stop + FADE_SAMPLES);
}

static void digital_to_stereo_downsample(struct psgplay *pp,
	const struct psgplay_digital *digital, const size_t count)
{
	struct psgplay_stereo stereo[4096];
	struct psgplay_stereo resample[ARRAY_SIZE(stereo)];
	size_t i = 0;

	while (i < count && !pp->errno_) {
		const size_t n = min(count - i, ARRAY_SIZE(stereo));
		
		pp->digital_to_stereo_callback.cb(pp, stereo, &digital[i], n,
			pp->digital_to_stereo_callback.arg);

		stereo_fade(stereo, n,
			pp->digital_buffer.total + i - count,
			pp->digital_buffer.stop);

		const size_t r = pp->stereo_downsample_callback.cb(resample,
			stereo, n, pp->stereo_downsample_callback.arg);

		pp->errno_ = buffer_stereo_sample(&pp->stereo_buffer, resample, r);

		i += n;
	}
}

static void psg_digital(const struct cf2149_ac *sample,
	size_t count, void *arg)
{
	struct psgplay *pp = arg;

	for (size_t i = 0; i < count; i++)
		if (!pp->errno_)
			pp->errno_ = buffer_digital_psg_sample(
				&sample[i], &pp->digital_buffer);
}

static void sound_digital(const struct sound_sample *sample,
	size_t count, void *arg)
{
	struct psgplay *pp = arg;

	for (size_t i = 0; i < count; i++)
		if (!pp->errno_)
			pp->errno_ = buffer_digital_sound_sample(
				&sample[i], &pp->digital_buffer);
}

static void mixer_digital(const struct mixer_sample *sample,
	size_t count, void *arg)
{
	struct psgplay *pp = arg;

	for (size_t i = 0; i < count; i++)
		if (!pp->errno_)
			pp->errno_ = buffer_digital_mixer_sample(
				&sample[i], &pp->digital_buffer);
}

static u32 parse_timer(const void *data, size_t size)
{
	struct sndh_timer timer;

	if (!sndh_tag_timer(&timer, data, size))
		return 0;

	return sndh_timer_to_u32(timer);
}

void psgplay_build_volume_table(struct psgplay *pp)
{
	double MaxVol = 65119.0;				/* Normal Mode Maximum value in table */
	double FOURTH2 = 1.19;					/* Fourth root of two from YM2149 */
	double WARP = 1.666666666666666667;		/* measured as 1.65932 from 46602 */

	double conductance;
	double conductance_[32];
	int	i, j, k;

	/**
	 * YM2149 and R8=1k follows (2^-1/4)^(n-31) better when 2 voices are
	 * summed (A+B or B+C or C+A) rather than individually (A or B or C):
	 *	 conductance = 2.0/3.0/(1.0-1.0/WARP)-2.0/3.0;
	 * When taken into consideration with three voices.
	 *
	 * Note that the YM2149 does not use laser trimmed resistances, thus
	 * has offsets that are added and/or multiplied with (2^-1/4)^(n-31).
	 */
	conductance = 2.0 / 3.0 / (1.0 - 1.0 / WARP) - 2.0 / 3.0; /* conductance = 1.0 */

	/**
	 * Because the YM current output (voltage source with series resistances)
	 * is connected to a grounded resistor to develop the output voltage
	 * (instead of a current to voltage converter), the output transfer
	 * function is not linear. Thus:
	 * 2.0*conductance_[n] = 1.0/(1.0-1.0/FOURTH2/(1.0/conductance + 1.0))-1.0;
	 */
	for (i = 31; i >= 1; i--) {
		conductance_[i] = conductance / 2.0;
		conductance = 1.0 / (1.0 - 1.0 / FOURTH2 / (1.0 / conductance + 1.0)) - 1.0;
	}
	conductance_[0] = 1.0e-8; /* Avoid divide by zero */

	/**
	 * YM2149 AC + DC components model:
	 * (Note that Maxvol is 65119 in Simoes' table, 65535 in Gerard's)
	 *
	 * Sum the conductances as a function of a voltage divider:
	 * Vout=Vin*Rout/(Rout+Rin)
	 */
	for (i = 0; i < 32; i++)
		for (j = 0; j < 32; j++)
			for (k = 0; k < 32; k++)
			{
				pp->dac[i][j][k] = (short)(0.5 + (MaxVol * WARP) / (1.0 +
					1.0 / (conductance_[i] + conductance_[j] + conductance_[k])));
			}

	/**
	 * YM2149 DC component model:
	 * R8=1k (pulldown) + YM//1K (pullup) with YM 50% duty PWM
	 * (Note that MaxVol is 46602 in Paulo Simoes Quartet mode table)
	 *
	 *	for (i = 0; i < 32; i++)
	 *		for (j = 0; j < 32; j++)
	 *			for (k = 0; k < 32; k++)
	 *			{
	 *				volumetable[i][j][k] = (ymu16)(0.5+(MaxVol*WARP)/(1.0 +
	 *					2.0/(conductance_[i]+conductance_[j]+conductance_[k])));
	 *			}
	 */
}

struct psgplay *psgplay_init(const void *data, size_t size,
	int track, int stereo_frequency)
{
	if (stereo_frequency &&
			(stereo_frequency < 1000 || 250000 < stereo_frequency))
		return NULL;

	struct psgplay *pp = calloc(1, sizeof(struct psgplay));
	if (!pp)
		return NULL;

	const u32 offset = MACHINE_PROGRAM;
	const struct machine_registers regs = {
		.d = { size, track, parse_timer(data, size) },
		.a = { offset },
	};

	const struct machine_ports ports = {
		.psg_sample = psg_digital,
		.sound_sample = sound_digital,
		.mixer_sample = mixer_digital,
		.arg = pp
	};
	
	psgplay_build_volume_table(pp);

	pp->downsample.stereo_frequency = stereo_frequency;
	pp->machine = &atari_st;
	pp->machine->init(data, size, offset, &regs, &ports);
	
	psgplay_digital_to_stereo_callback(pp,
		psgplay_digital_to_stereo_empiric, NULL);

	psgplay_stereo_downsample_callback(pp,
		stereo_downsample, &pp->downsample);
	

	return pp;
}

static size_t digital_buffer_min_count(struct digital_buffer *db)
{
	return min3(db->count.psg, db->count.sound, db->count.mixer);
}

static size_t digital_buffer_max_count(struct digital_buffer *db)
{
	return max3(db->count.psg, db->count.sound, db->count.mixer);
}

static void digital_buffer_shift(struct digital_buffer *db)
{
	const size_t n = digital_buffer_max_count(db) - db->index;

	if (n)
		memmove(&db->sample[0], &db->sample[db->index],
			n * sizeof(*db->sample));

	db->count.psg -= db->index;
	db->count.sound -= db->index;
	db->count.mixer -= db->index;

	db->index = 0;
}

static ssize_t psgplay_read_digital__(struct psgplay *pp,
	struct psgplay_digital *buffer, size_t count)
{
	struct digital_buffer *db = &pp->digital_buffer;
	size_t index = 0;

	if (db->stop) {
		if (db->total >= db->stop)
			return 0;

		count = min(count, db->stop - db->total);
	}

	cpu_instruction_callback(
		pp->instruction_callback.cb,
		pp->instruction_callback.arg);

	while (index < count) {
		if (db->index == digital_buffer_min_count(db)) {
			digital_buffer_shift(db);

			while (!digital_buffer_min_count(db))
				if (pp->errno_) {
					errno = pp->errno_;
					return -1;
				} else if (!pp->machine->run()) {
					errno = -EIO;
					return -1;
				}
		}

		const size_t n = min(count - index,
			digital_buffer_min_count(db) - db->index);

		if (buffer != NULL)
			memcpy(&buffer[index], &db->sample[db->index],
				n * sizeof(*buffer));

		index += n;
		db->index += n;
		db->total += n;
	}

	return index;
}

ssize_t psgplay_read_stereo(struct psgplay *pp,
	struct psgplay_stereo *buffer, size_t count)
{
	struct stereo_buffer *sb = &pp->stereo_buffer;
	size_t index = 0;

	if (!pp->downsample.stereo_frequency)
		return -EINVAL;

	while (index < count) {
		if (sb->index == sb->count) {
			sb->index = 0;
			sb->count = 0;

			if (pp->errno_) {
				errno = pp->errno_;
				return -1;
			}

			struct psgplay_digital d[4096];
			const ssize_t n = psgplay_read_digital__(
				pp, d, ARRAY_SIZE(d));

			if (n < 0)
				return n;
			else if (!n)
				return index;

			digital_to_stereo_downsample(pp, d, n);
		}

		const size_t n = min(count - index, sb->count - sb->index);

		if (buffer != NULL)
			memcpy(&buffer[index], &sb->sample[sb->index],
				n * sizeof(*buffer));

		index += n;
		sb->index += n;
		sb->total += n;
	}

	return index;
}

ssize_t psgplay_read_digital(struct psgplay *pp,
	struct psgplay_digital *buffer, size_t count)
{
	if (pp->downsample.stereo_frequency)
		return -EINVAL;

	return psgplay_read_digital__(pp, buffer, count);
}

void psgplay_free(struct psgplay *pp)
{
	if (!pp)
		return;

	free(pp->stereo_buffer.sample);
	free(pp->digital_buffer.sample);
	free(pp);
}

void psgplay_stop_digital_at_sample(struct psgplay *pp, size_t index)
{
	pp->digital_buffer.stop =
		max(index, pp->digital_buffer.total + FADE_SAMPLES);
}

void psgplay_stop_at_time(struct psgplay *pp, float time)
{
	psgplay_stop_digital_at_sample(pp, max(0.0, 250e3 * time));
}

void psgplay_stop(struct psgplay *pp)
{
	psgplay_stop_at_time(pp, 0);
}

void psgplay_instruction_callback(struct psgplay *pp,
	void (*cb)(uint32_t pc, void *arg), void *arg)
{
	pp->instruction_callback.cb = cb;
	pp->instruction_callback.arg = arg;
}
