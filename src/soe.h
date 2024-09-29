#ifndef SOE_H
#define SOE_H

#include "stdint.h"
#include "stdbool.h"


// NOTE: calculates prames based on Sieve of Eratosthenes algorythm
// https://en.wikipedia.org/wiki/Sieve_of_Eratosthenes


struct SoeCache {
	uint64_t _max;
	uint8_t *_data;
};

struct SoeCache soe_init(uint64_t max);
void soe_deinit(const struct SoeCache cache);
bool soe_is_prime(const struct SoeCache cache, uint64_t num);

#endif  // SOE_H
