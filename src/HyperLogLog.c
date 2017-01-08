#include <limits.h>
#include <stddef.h>
#include <stdlib.h>
#include <x86intrin.h>
#include "HyperLogLog.h"

/**
 * @file HyperLogLog.c
 *
 * Contains the implementations of the functions defined in HyperLogLog.h, as well as some static helper functions.
 *
 * **Compile with `-mbmi`**
 */

// TODO: cover the case that bit manipulation instructions aren't available

#if CHAR_BIT != 8
#error Fuck it! Get a normal computer, dude!
#endif

#define TZCNT16(x) __tzcnt_u16(x)
#define TZCNT64(x) __tzcnt_u64(x)

#ifndef __x86_64__
inline uint64_t __tzcnt_u64 (uint64_t x)
{
	return x & 0x00000000FFFFFFFF ? __tzcnt_u32(x) : __tzcnt_u32((uint32_t)(x >> 32)) + 32;
}
#endif



/**
 * Returns the `b` least significant bits of `x`.
 *
 * @param b: b < sizeof(size_t) * CHAR_BIT
 */
static inline size_t getFirstBBits(size_t x, size_t b)
{
	return x & ((1 << b) - 1);
}

static inline uint8_t getMediumReg(uint32_t block, unsigned char index)
{
	return (uint8_t)( (block & (0x3F << index * 6)) >> index * 6 );
}

static inline uint8_t getSmallReg(uint8_t block, unsigned char index)
{
	return (uint8_t)( (block & (0xF << index * 4)) >> index * 4 );
}

/**
 * Returns the greatest value of `a` and `b`.
 */
static inline size_t max(size_t a, size_t b)
{
	return a > b ? a : b;
}

/**
 * Same as `max()` but with `uint8_t`.
 */
static inline uint8_t maxb(uint8_t a, uint8_t b)
{
	return a > b ? a : b;
}

/**
 * Returns the lowers value of `a` and `b`.
 */
static inline uint8_t minb(uint8_t a, uint8_t b)
{
	return a < b ? a : b;
}

static inline void setMediumReg(uint32_t *block, uint8_t reg, unsigned char index)
{
	*block &= ~(0x3F << index * 6);
	*block |= reg << index * 6;
}

static inline void setSmallReg(uint8_t *block, uint8_t reg, unsigned char index)
{
	*block &= ~(0xF << index * 4);
	*block |= reg << index * 4;
}

/**
 * Returns the one-based index of the rightmost 1-bit in `buffer`.
 */
static uint8_t rho(size_t r, void *buffer)
{
	// pointer used to dereference `buffer` as different types
	union
	{
		uint16_t *u16;
		uint64_t *u64;
	} ptr;

	ptr.u16 = buffer;

	switch (r)
	{
		case SMALL:
			*ptr.u16 |= (uint16_t)0xC000;
			return (uint8_t)(TZCNT16(*ptr.u16) + 1);

		case MEDIUM:
			*ptr.u64 |= (uint64_t)0xC000000000000000;
			return (uint8_t)(TZCNT64(*ptr.u64) + 1);

		case LARGE:
			ptr.u64[3] |= (uint64_t)0xC000000000000000;

			uint64_t result = 0;
			for (int i = 0; i < 4; i++)
			{
				if (ptr.u64[i])
				{
					result += TZCNT64(ptr.u64[i]);
					break;
				}
				else
				{
					result += 64;
				}
			}

			return (uint8_t)(result + 1);

		default:
			return 0;
	}
}

/**
 * Updates the register at a given index, if the tailing zero count is greater than the old one.
 */
static void updateReg(void *blocks, unsigned char r, size_t index, uint8_t n_tailing_zeros)
{
	unsigned char reg_index;
	size_t byte_index;

	uint8_t reg;

	// get register
	switch (r)
	{
		case SMALL:
			byte_index = index / 2;
			reg_index = (unsigned char)(index % 2);

			uint8_t sblock = *(uint8_t*)(blocks + byte_index);
			reg = getSmallReg(sblock, reg_index);
			break;
		case MEDIUM:
			byte_index = index / 4 * 3;
			reg_index = (unsigned char)(index % 4);

			uint32_t mblock = *(uint32_t*)(blocks + byte_index);
			reg = getMediumReg(mblock, reg_index);
			break;
		case  LARGE:
			byte_index = index;
			reg_index = 0;

			reg = *(uint8_t*)(blocks + byte_index);
			break;
		default:
			return;
	}

	reg = minb(maxb(reg, n_tailing_zeros), (uint8_t)((1 << r) - 1));

	// set register
	switch (r)
	{
		case SMALL:
			setSmallReg((uint8_t*)(blocks + byte_index), reg, reg_index);
			break;
		case MEDIUM:
			setMediumReg((uint32_t*)(blocks + byte_index), reg, reg_index);
			break;
		case LARGE:
			*(uint8_t*)(blocks + byte_index) = reg;
			break;
	}
}



int hllInit(struct HyperLogLog *this, unsigned char type, unsigned char b, void (*hash)(void *, size_t, void *))
{
	if (this == NULL)
		return 1;

	if (type != SMALL && type != MEDIUM && type != LARGE)
		return 2;

	if (b >= sizeof(size_t) * CHAR_BIT)
		return 3;

	if (hash == NULL)
		return 4;

	this->r = type;
	this->b = b;
	this->hash = hash;

	size_t n_bytes = (size_t)1 << b;
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

	this->data = calloc(n_bytes, sizeof(char));
	if (this->data == NULL)
		return -1;

	return 0;
}

void hllFree(struct HyperLogLog *this)
{
	if (this != NULL)
	{
		free(this->data);
	}
}

void hllAdd(struct HyperLogLog *this, void *item)
{
	if (this == NULL)
		return;

	// number of bytes that are used for the tailing zero count
	size_t tzcnt_length;

	switch (this->r)
	{
		case SMALL:  tzcnt_length = sizeof(uint16_t); break;
		case MEDIUM: tzcnt_length = sizeof(uint64_t); break;
		case LARGE:  tzcnt_length = 4 * sizeof(uint64_t); break;
		default: return;
	}

	char buffer[sizeof(size_t) + tzcnt_length];
	void *hash = buffer;

	this->hash(item, sizeof(hash), hash);

	size_t reg_index = *(size_t *)(hash + tzcnt_length);
	reg_index = getFirstBBits(reg_index, this->b);

	updateReg(this->data, this->r, reg_index, rho(this->r, hash));
}