#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "./arr.h"

// TODO: it is possible to decrease MEM usage 16x times:
// - skip every even number (2x)
// - bool uses 8bit, while we only need one (8x)
// However, I'm not sure how much this would decrease mem access speed.
ARR_STRUCT(SoeCache, bool, uint64_t);

// https://en.wikipedia.org/wiki/Sieve_of_Eratosthenes
struct SoeCache soe_populate(uint64_t max);
struct SoeCache soe_populate(uint64_t max)
{
	struct SoeCache out;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wuseless-cast"
	// TODO: another possible optimization here:
	// - memset to 0 could be faster
	// - and skipping even indexes would also be faster (but this one would require a loop)
	arr_init_memset(out, max, true);
#pragma GCC diagnostic pop
	uint64_t i = 3;
	for (; (i * i) <= max; i += 2) {
		const bool new = arr_at_val(out, i);
		if (!new)
			continue;
		for (uint64_t p = i * 2; p <= max; p += i) {
			bool *ref = arr_at_ref(out, p);
			*ref = false;
		}
	}
	return out;
}

bool soe_is_prime(const struct SoeCache cache, uint64_t num);
bool soe_is_prime(const struct SoeCache cache, uint64_t num)
{
	dbg_assert(num < arr_size(cache));
	const bool out = (num % 2 == 0) ? false : arr_at_val(cache, num);
	fprintf(stderr, "%lu is a %s number\n", num, out ? "PRIME" : "composite");
	return out;
}

int
main(void)
{
	const struct SoeCache cache = soe_populate(1920 * 1080);
	(void)soe_is_prime(cache, 2);
	(void)soe_is_prime(cache, 11);
	(void)soe_is_prime(cache, 15);
	(void)soe_is_prime(cache, 2113);
	(void)soe_is_prime(cache, 2114);
	(void)soe_is_prime(cache, 1111001);
	return EXIT_SUCCESS;
}

