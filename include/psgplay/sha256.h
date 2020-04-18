// SPDX-License-Identifier: GPL

#ifndef PSGPLAY_SHA256_H
#define PSGPLAY_SHA256_H

#include "psgplay/types.h"

#define SHA256_DIGEST_SIZE      32
#define SHA256_BLOCK_SIZE       64

struct sha256_state {
	u32 state[SHA256_DIGEST_SIZE / 4];
	u64 count;
	u8 buf[SHA256_BLOCK_SIZE];
};

void sha256_init(struct sha256_state *sctx);
void sha256_update(struct sha256_state *sctx, const void *buf, size_t size);
void sha256_final(struct sha256_state *sctx, u8 *hash);

struct sha256 {
	u8 d[SHA256_DIGEST_SIZE];
};

struct sha256 sha256_digest(const void *buf, size_t size);

int sha256_compare(struct sha256 a, struct sha256 b);

#endif /* PSGPLAY_SHA256_H */
