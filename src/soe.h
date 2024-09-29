#ifndef SOE_H
#define SOE_H

#include "stdint.h"
#include "stdbool.h"


// NOTE: calculates prames based on Sieve of Eratosthenes algorythm
// https://en.wikipedia.org/wiki/Sieve_of_Eratosthenes

#define SOE_OPTIMIZED_MEM

struct SoeCache {
	uint8_t *_data;
	uint64_t _len;
	uint64_t _cap;
};

struct SoeCache soe_init(uint64_t max);
void soe_deinit(const struct SoeCache cache);
bool soe_is_prime(const struct SoeCache cache, uint64_t num);

#endif  // SOE_H
