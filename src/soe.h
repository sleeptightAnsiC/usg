#ifndef _SOE_H
#define _SOE_H

#include "./typ.h"

/* SOE
 * Calculates primes based on Sieve of Eratosthenes
 * Note that this algorithm requires caching
 * and so it allocates memory on the heap
 * https://en.wikipedia.org/wiki/Sieve_of_Eratosthenes
 *
 * SOE_OPTIMIZED_MEM
 *	This flag changes how soe_cache stores the _data.
 *	Defining it results in 16x less memory usage,
 *	but accessing cache becomes slower.
 */

// #define SOE_OPTIMIZED_MEM

struct soe_cache {
	u64 _max;
	u8 *_data;
};

struct soe_cache soe_init(u64 max);
void soe_deinit(const struct soe_cache cache);
b8 soe_is_prime(const struct soe_cache cache, u64 num);

#endif  // _SOE_H
