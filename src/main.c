#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "./arr.h"

// https://en.wikipedia.org/wiki/Sieve_of_Eratosthenes

// TODO: it is possible to decrease MEM usage 16x times:
// - skip every even number (2x)
// - bool uses 8bit, while we only need one (8x)
// However, I'm not sure how much this would decrease mem access speed.
ARR_STRUCT(SoeCache, bool, uint64_t);

struct SoeCache soe_populate(uint64_t max)
{
	dbg_assert(max >= 1);
	dbg_assert(max <= UINT64_MAX - 1);
	max += 1;
	struct SoeCache out;
	// TODO: skipping even indexes could be faster
	arr_init_null(out, max);
	for (uint64_t i = 3; (i * i) < max; i += 2) {
		const bool new = arr_at_val(out, i);
		if (new)
			continue;
		for (uint64_t p = i * 2; p < max; p += i) {
			bool *ref = arr_at_ref(out, p);
			*ref = true;
		}
	}
	return out;
}

bool soe_is_prime(const struct SoeCache cache, uint64_t num)
{
	dbg_assert(num < arr_size(cache));
	const bool out = (num % 2 != 0) && !arr_at_val(cache, num);
	fprintf(stderr, "%lu is a %s number\n", num, out ? "PRIME" : "composite");
	return out;
}

int
main(void)
{
	const struct SoeCache cache = soe_populate(1920 * 1080);
	// const struct SoeCache cache = soe_populate(15);
	(void)soe_is_prime(cache, 2);
	(void)soe_is_prime(cache, 11);
	(void)soe_is_prime(cache, 15);
	(void)soe_is_prime(cache, 2113);
	(void)soe_is_prime(cache, 2114);
	(void)soe_is_prime(cache, 1111001);
	(void)soe_is_prime(cache, 1920 * 1080);
	return EXIT_SUCCESS;
}

