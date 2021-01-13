// SPDX-License-Identifier: LGPL-2.1

#ifndef _FUJILIBC_STRING_H
#define _FUJILIBC_STRING_H

#include <stddef.h>

void *memcpy(void *dst, const void *src, size_t nbytes);

void *memset(void *s, int c, size_t n);;

int strcmp(const char *s1, const char *s2);

size_t strlen(const char *s);

int strncmp(const char *s1, const char *s2, size_t n);

char *strncpy(char *dst, const char *src, size_t n);

const char *strerror(int errnum);

char *strdup(const char *s);

char *strndup(const char *s, size_t n);

char *strstr(const char *haystack, const char *needle);

#endif /* _FUJILIBC_STRING_H */
