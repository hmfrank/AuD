#ifndef AUD_TZCNT_H
#define AUD_TZCNT_H

#include <stddef.h>

#ifndef NO_BMI
#include <x86intrin.h>
#endif

/**
 * @file tzcnt.h
 *
 * Contains the macro definition TZCNT16(x), TZCNT(32) and TZCNT64(x) that count the number of tailing zeros in a binraty
 * number.
 *
 * **Compile with `-mbmi` or if that doesn't work with `-D NO_BMI`**
 */

#define TZCNT16(x) __tzcnt_u16(x)
#define TZCNT32(x) __tzcnt_u32(x)
#define TZCNT64(x) __tzcnt_u64(x)

#ifdef NO_BMI
static inline uint16_t __tzcnt_u16(uint16_t x)
{
	for (int bit_index = 0; bit_index < 16; bit_index++)
	{
		if (x & ((uint16_t)1 << bit_index))
			return bit_index;
	}

	return 32;
}

static inline uint32_t __tzcnt_u32(uint32_t x)
{
	for (int bit_index = 0; bit_index < 32; bit_index++)
	{
		if (x & ((uint32_t)1 << bit_index))
			return bit_index;
	}

	return 32;
}

static inline uint64_t __tzcnt_u64(uint64_t x)
{
	for (int bit_index = 0; bit_index < 64; bit_index++)
	{
		if (x & ((uint64_t)1 << bit_index))
			return bit_index;
	}

	return 64;
}
#endif

#if !defined(__x86_64__) && !defined(NO_BMI)
static inline uint64_t __tzcnt_u64(uint64_t x)
{
	return x & 0x00000000FFFFFFFF ? __tzcnt_u32(x) : __tzcnt_u32((uint32_t)(x >> 32)) + 32;
}
#endif

#endif //AUD_TZCNT_H
