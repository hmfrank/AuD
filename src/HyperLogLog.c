#include <stddef.h>
#include <stdlib.h>
#include "HyperLogLog.h"

/**
 * @file HyperLogLog.c
 *
 * Contains the implementations of the functions defined in HyperLogLog.h.
 */

static inline size_t max(size_t a, size_t b)
{
	return a > b ? a : b;
}

int hllInit(struct HyperLogLog *this, unsigned char type, unsigned char k, void (*hash)(const void *, size_t h, byte *))
{
	if (this == NULL)
		return 1;

	if (type != SMALL && type != MEDIUM && type != LARGE)
		return 2;

	if ((size_t)1 << k == 0)
		return 3;

	if (hash == NULL)
		return 4;

	this->m = type;
	this->k = k;
	this->hash = hash;

	size_t n_bytes = (size_t)1 << k;
	switch (type)
	{
		case SMALL:
			n_bytes = max(n_bytes, 2);
			n_bytes /= 2;
			break;
		case MEDIUM:
			n_bytes = max(n_bytes, 4);
			n_bytes /= 4;
			n_bytes *= 3;
			break;
	}

	this->buckets = calloc(n_bytes, sizeof(byte));
	if (this->buckets == NULL)
		return -1;

	return 0;
}

void hllFree(struct HyperLogLog *this)
{
	if (this != NULL)
	{
		free(this->buckets);
	}
}