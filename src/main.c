#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "./arr.h"


// https://en.wikipedia.org/wiki/Trial_division
void tdv_populate(uint64_t max);
void tdv_populate(uint64_t max)
{
	ARR_STRUCT(, uint64_t, int) primes;
	arr_init_cap(primes, 20);
	arr_push_back(primes, 2);
	for (uint64_t i = 3; i <= max; i += 2) {
		bool new = true;
		for (int p = 0; p < arr_size(primes) && (uint64_t)(p * p) < i; ++p) {
			const uint64_t val = arr_at_val(primes, p);
			dbg_assert(val > 0);
			if (i % val == 0) {
				new = false;
				break;
			}
		}
		if (new) {
			arr_push_back(primes, i);
			fprintf(stderr, "New prime found!\t%lu\n", arr_back_val(primes));
		}
	}
}

// https://en.wikipedia.org/wiki/Sieve_of_Eratosthenes
void soe_populate(uint64_t max);
void soe_populate(uint64_t max)
{
	// TODO: it is possible to decrease MEM usage 16x times:
	// - skip every even number (2x)
	// - bool uses 8bit, while we only need one (8x)
	// However, I'm not sure how much this would decrease the speed.
	ARR_STRUCT(, bool, uint64_t) lookup;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wuseless-cast"
	// TODO: another possible optimization here:
	// - memset to 0 could be faster
	// - and skipping even indexes would also be faster (but this one would require a loop)
	arr_init_memset(lookup, max, true);
#pragma GCC diagnostic pop
	ARR_STRUCT(, uint64_t, int) primes;
	arr_init_cap(primes, 20);
	arr_push_back(primes, 2);
	uint64_t i = 3;
	for (; (i * i) <= max; i += 2) {
		const bool new = arr_at_val(lookup, i);
		if (new) {
			arr_push_back(primes, i);
			// fprintf(stderr, "New prime found!\t%lu\n", arr_back_val(primes));
			for (uint64_t p = i * 2; p <= max; p += i) {
				bool *ref = arr_at_ref(lookup, p);
				*ref = false;
			}
		}
	}
	for (; i <= max; i += 2) {
		const bool val = arr_at_val(lookup, i);
		if (val) {
			arr_push_back(primes, i);
			// fprintf(stderr, "New prime found!\t%lu\n", arr_back_val(primes));
		}
	}
	fprintf(stderr, "New prime found!\t%lu\n", arr_back_val(primes));
}

int
main(void)
{
	soe_populate(1920 * 1080);
	return EXIT_SUCCESS;
}

