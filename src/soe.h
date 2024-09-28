
#include "stdint.h"
#include "stdbool.h"
#include "./arr.h"

#ifndef SOE_H
#define SOE_H

// https://en.wikipedia.org/wiki/Sieve_of_Eratosthenes

// TODO: it is possible to decrease MEM usage 16x times:
// - skip every even number (2x)
// - bool uses 8bit, while we only need one (8x)
// However, I'm not sure how much this would decrease mem access speed.
ARR_STRUCT(SoeCache, bool, uint64_t);

struct SoeCache soe_populate(uint64_t max);
bool soe_is_prime(const struct SoeCache cache, uint64_t num);

#endif  // SOE_H
