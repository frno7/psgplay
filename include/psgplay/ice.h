// SPDX-License-Identifier: GPL-2.0

#ifndef PSGPLAY_ICE_H
#define PSGPLAY_ICE_H

#include <stdbool.h>
#include <stddef.h>

#define ICE_HEADER_SIZE 12

/**
 * ice_identify - is data ICE compressed?
 * @data: compressed data
 * @size: size in bytes of compressed data, or greater
 *
 * Return: %true if data seems to be ICE compressed, otherwise %false
 */
bool ice_identify(const void *data, size_t size);

/**
 * ice_crunched_size - determine size of ICE compressed data
 * @data: compressed data
 * @size: size in bytes of compressed data, or greater
 *
 * Return: size in bytes of compressed data, or zero on failure
 */
size_t ice_crunched_size(const void *data, size_t size);

/**
 * ice_decrunched_size - determine size of ICE decompressed data
 * @data: compressed data
 * @size: size in bytes of compressed data, or greater
 *
 * Return: size in bytes of decompressed data, or zero on failure
 */
size_t ice_decrunched_size(const void *data, size_t size);

/**
 * ice_decrunch - ICE decompress data
 * @out: decompressed output data FIXME: Does inplace decompression work?
 * @in: compressed input data
 * @insize: size in bytes of compressed data, or greater
 *
 * Note that the @out buffer must be large enough to contain all decompressed
 * data. Use &ice_decrunched_size to determine the size.
 *
 * Return: size in bytes of decompressed data, or -1 on failure
 */
ssize_t ice_decrunch(void *out, const void *in, size_t insize);

#endif /* PSGPLAY_ICE_H */
