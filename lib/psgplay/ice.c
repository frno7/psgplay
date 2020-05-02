// SPDX-License-Identifier: GPL-2.0

#include <string.h>

#include "internal/build-assert.h"
#include "internal/types.h"

#include "psgplay/ice.h"

struct ice_decrunch_state
{
	u8 *unpacked_stop;
	u8 *unpacked;
	const u8 *packed;
	int bits;
};

struct ice_u32 {
	u8 d[4];
};

struct ice_header {
	char magic[4];
	struct ice_u32 crunched_size;
	struct ice_u32 decrunched_size;
};

static u32 ice_u32(struct ice_u32 value)
{
	return (value.d[0] << 24) |
	       (value.d[1] << 16) |
	       (value.d[2] <<  8) |
		value.d[3];
}

static const struct ice_header *ice_header(const void *data, size_t size)
{
	const struct ice_header *h = data;

	BUILD_BUG_ON(sizeof(*h) != 12);

	if (size < sizeof(*h))
		return NULL;

	if (h->magic[0] != 'I' ||
	    h->magic[1] != 'C' ||
	    h->magic[2] != 'E' ||
	    h->magic[3] != '!')
		return NULL;

	if (size < ice_u32(h->crunched_size))
		return NULL;

	return data;
}

bool ice_identify(const void *data, size_t size)
{
	return ice_header(data, size) != NULL;
}

size_t ice_crunched_size(const void *data, size_t size)
{
	const struct ice_header *h = ice_header(data, size);

	return h ? ice_u32(h->crunched_size) : 0;
}

size_t ice_decrunched_size(const void *data, size_t size)
{
	const struct ice_header *h = ice_header(data, size);

	return h ? ice_u32(h->decrunched_size) : 0;
}

static void memcpybwd(u8 *to, const u8 *from, size_t n)
{
	to += n;
	from += n;

	while (n-- > 0)
		*--to = *--from;
}

static int get_bit(struct ice_decrunch_state *state)
{
	int bit = (state->bits & 0x80) != 0;

	state->bits = (state->bits << 1) & 0xff;
	if (state->bits == 0)
	{
		state->bits = *--state->packed;

		bit = (state->bits & 0x80) != 0;
		state->bits = ((state->bits << 1) & 0xff) + 1;
	}

	return bit;
}

static int get_bits(struct ice_decrunch_state *state, int n)
{
	int bits = 0;

	while (n--)
		bits = bits << 1 | get_bit(state);

	return bits;
}

static int get_depack_length(struct ice_decrunch_state *state)
{
	static const int bits_to_get[]   = { 0, 0, 1, 2, 10 };
	static const int number_to_add[] = { 2, 3, 4, 6, 10 };
	int i, length;
	int bits;

	for (i = 0; i < 4; i++)
	{
		if (get_bit(state) == 0)
			break;
	}

	bits = bits_to_get[i];
	if (bits > 0)
		length = get_bits(state, bits);
	else
		length = 0;

	return length + number_to_add[i];
}

static int get_depack_offset(struct ice_decrunch_state *state, int length)
{
	static const int bits_to_get[] =   {  8,  5,  12 };
	static const int number_to_add[] = { 31, -1, 287 };
	int offset;
	int i;

	if (length == 2) {
		if (get_bit(state))
			return get_bits(state, 9) + 0x3f;
		else
			return get_bits(state, 6) - 1;
	}

	for (i = 0; i < 2; i++)
	{
		if (get_bit(state) == 0)
			break;
	}

	offset = get_bits(state, bits_to_get[i]) + number_to_add[i];

	return offset < 0 ? offset + 2 - length : offset;
}

static int get_direct_length(struct ice_decrunch_state *state)
{
	static const int bits_to_get[] =   { 1, 2, 2, 3,    8,     15 };
	static const int all_ones[] =      { 1, 3, 3, 7, 0xff, 0x7fff };
	static const int number_to_add[] = { 1, 2, 5, 8,   15,    270, 270 };
	int i, n;

	for (i = 0; i < 6; i++)
	{
		n = get_bits(state, bits_to_get[i]);

		if (n != all_ones[i])
			break;
	}

	return n + number_to_add[i];
}

static bool normal_bytes(struct ice_decrunch_state *state)
{
	for (;;) {
		if (get_bit(state)) {
			const int length = get_direct_length(state);

			state->packed -= length;
			state->unpacked -= length;

			if (state->unpacked < state->unpacked_stop)
				return false;

			memcpy(state->unpacked, state->packed, length);
		}

		if (state->unpacked > state->unpacked_stop) {
			const int length = get_depack_length(state);
			const int offset = get_depack_offset(state, length);

			state->unpacked -= length;

			if (state->unpacked < state->unpacked_stop)
				return false;

			memcpybwd(state->unpacked,
				 &state->unpacked[length + offset], length);
		} else
			return true;
	}
}

ssize_t ice_decrunch(void *out, const void *in, size_t insize)
{
	const size_t unpacked_length = ice_decrunched_size(in, insize);
	const size_t packed_length = ice_crunched_size(in, insize);
	const u8 *p = in;
	u8 *u = out;

	if (packed_length && unpacked_length) {
		struct ice_decrunch_state state = {
			.unpacked_stop = out,
			.packed = &p[packed_length - 1],
			.bits = p[packed_length - 1],
			.unpacked = &u[unpacked_length],
		};

		if (!normal_bytes(&state))
			return -1;
	}

	return unpacked_length;
}
