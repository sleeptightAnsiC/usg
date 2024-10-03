#ifndef SOE_H
#define SOE_H

#include <stdint.h>
#include <stdbool.h>

/* SOE
 * Calculates primes based on Sieve of Eratosthenes
 * Note that this algorithm requires caching
 * and so it allocates memory on the heap
 * https://en.wikipedia.org/wiki/Sieve_of_Eratosthenes
 *
 * SOE_OPTIMIZED_MEM
 *	This flag changes how cache stores the values.
 *	Defining it results in 16x less memory usage,
 *	but accessing cache becomes slower.
 */

// #define SOE_OPTIMIZED_MEM

struct SoeCache {
	uint64_t _max;
	uint8_t *_data;
};

struct SoeCache soe_init(uint64_t max);
void soe_deinit(const struct SoeCache cache);
bool soe_is_prime(const struct SoeCache cache, uint64_t num);

#endif  // SOE_H
