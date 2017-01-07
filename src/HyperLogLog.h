#ifndef AUD_HYPERLOGLOG_H
#define AUD_HYPERLOGLOG_H

#include <stdint.h>

/**
 * @file HyperLogLog.h
 *
 * Contains the struct definition of 'struct HyperLogLog` as well as related function prototypes.
 */

/**
 * Specifies the bucket size of a HyperLogLog-Set in bits.
 *
 * @see struct HyperLogLog
 */
enum HyperLogLogBucketSize
{
	SMALL = 4,
	MEDIUM = 6,
	LARGE = 8
};

/**
 * Stores probabilistic information for counting the number of unique elements in a set.
 * HyperLogLog is suited extremely well for counting very huge sets with decent precision, since the memory usage is
 * \f$ O(log(log(c)) \f$, where \f$c\f$ is the maximum number of items in the set.
 *
 * In this documentation, I'll explain what you need to know to _use_ this counter.
 * If you're interested in how it _works_, take a look at the Wikipedia articles or at the source code.
 *
 * There are two main parameters that influence how the counter performs:
 * the **bucket size** \f$m\f$ (in bits) and the **number of buckets** \f$n\f$, with
 * \f$m \in \{4, 6, 8\}\f$ and
 * \f$n = 2^k, k \in \mathbb{N}\f$ (due to implementation optimizations).
 * Both, \f$m\f$ and \f$k\f$, have to be chosen, when creating a new empty counter using `hllInit()`.
 *
 * #### Choosing \f$m\f$ and \f$k\f$
 *
 * \f$m\f$ has a huge influence on the maximum number of items \f$c\f$ in the set.
 * Specifically \f$c = 2^{k + 2^m - 1}\f$.
 * This doesn't stop you from adding more than \f$c\f$ items, but if you do, the counting result can get very inaccurate.<br/>
 * As a rule of thumb, if you want to count in the range of millions and billions, \f$m = 4\f$ should be fine.
 * \f$m = 6\f$ is good, if you stay below \f$10^{21}\f$.
 * \f$m = 8\f$ is for virtually infinitely expandable sets (calculate it through if you're curious).
 *
 * The other parameter, \f$k\f$, influences how much the counted value might deviate from the actual number of items
 * added, as well as how much memory is needed.
 * The error can be calculated with \f$\frac{1.04}{\sqrt{2^k}}\f$.<br/>
 * The memory used can be calculated with \f$ m \cdot 2^k bits\f$
 *
 * #### Formular Summary
 *
 * \f$2^k\f$ buckets of \f$m\f$ bits each.
 *
 * \f$c = 2^{k + 2^m - 1}\f$ (maximum cardinality)
 *
 * \f$e = \frac{1.04}{\sqrt{2^k}}\f$ (error)
 *
 * \f$b = m \cdot 2^k\f$ (memory usage in bits)
 *
 * #### If you don't wanna read, skip to this section.
 *
 * In case you are totally confused now, choose \f$m = 6\f$ and \f$k = 11\f$. This will probably work for most applications.
 * You can count up to \f$10^{22}\f$ unique items, while using about 1.5KB of memory and only being about 2.3% off.
 *
 * You should always call `hllInit()` before and `hllFree()` after using this structure.
 *
 * The members of this struct should not be accessed directly.
 * Allways use this struct through the provided methods that start with "hll".
 *
 * @see http://algo.inria.fr/flajolet/Publications/FlFuGaMe07.pdf
 * @see https://en.wikipedia.org/wiki/HyperLogLog
 * @see https://en.wikipedia.org/wiki/Flajolet%E2%80%93Martin_algorithm
 * @see hllInit()
 * @see hllFree()
 * @see hllAdd()
 * @see hllCount()
 */
struct HyperLogLog
{
	/**
	 * The bucket size used. Only values of `enum HyperLogLogBucketSize` are accepted.
	 */
	unsigned char m;

	/**
	 * The number of bits needed to index one bucket. In other words, the number of buckets is \f$2^k\f$.
	 */
	unsigned char k;

	/**
	 * Pointer to a hash function that is applied on the items of the set. This function is called every time an item is
	 * added to the set (every time `hllAdd()` is called).
	 *
	 * The generated hash should have a uniform distribution of 1- and 0-bits.
	 *
	 * @param item: The item that gets added to the set.
	 * @param h: How many hash bytes the function shall generate.
	 * @param buffer: points the buffer to store the hash in.
	 */
	void (*hash)(void *item, size_t h, void *buffer);

	/**
	 * Points to the actual data.
	 */
	void *blocks;
};

/**
 * Initializes an empty HyperLogLog.
 *
 * `m` and `k` are the parameters I talked about in the description of `struct HyperLogLog`.
 * If you don't know what to them, take a look at that description.
 *
 * @param _this Points to the set to be initialized.
 * @param hash Pointer to the hash function used.
 * @return status code with the following meanings:<br/>
 *  * 0 = success
 *  * 1 = invalid argument `_this`
 *  * 2 = invalid argument `m`
 *  * 3 = invalid argument `k`
 *  * 4 = invalid argument `hash`
 *  * -1 = malloc error
 *
 *  @see HyperLogLog
 */
int hllInit(struct HyperLogLog *_this, unsigned char m, unsigned char k, void (*hash)(void*, size_t, void*));

/**
 * Frees all the memory used by a HyperLogLog structure. You can not use the struct after you passed it to this function.
 *
 * If `_this` is `NULL`, nothing happens.
 *
 * @param _this Points to the structure to be freed. This pointer itself is not freed.
 */
void hllFree(struct HyperLogLog *_this);

/**
 * Adds an item to the set.
 *
 * @param _this Points to the HyperLogLog structure, that counts the set.
 * @param item The item to add. The pointer is not needed after this function returns, so you can
 * free or do anything to it, without concern.
 */
void hllAdd(struct HyperLogLog *_this, void *item);

/**
 * Counts the number of unique items added to the set.
 *
 * @param _this Points the HyperLogLog structure, that counts the set.
 * @return An approximation of the number of unique elements in the set, or NaN on error.
 */
double hllCount(struct HyperLogLog *_this);

#endif //AUD_HYPERLOGLOG_H
