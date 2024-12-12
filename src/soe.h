#ifndef _SOE_H
#define _SOE_H

#include "./typ.h"

/* SOE
 * Calculates primes based on Sieve of Eratosthenes.
 * Note that this algorithm requires caching
 * and so it allocates memory on the heap.
 * https://en.wikipedia.org/wiki/Sieve_of_Eratosthenes
 */


struct soe_cache {
	u64 _cap;
	u8 *_data;
};

b8 soe_init(struct soe_cache *cache, u64 max);
b8 soe_deinit(struct soe_cache *cache);
b8 soe_is_prime(const struct soe_cache *cache, u64 num);

#endif  // _SOE_H
