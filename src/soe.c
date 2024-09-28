#include "./soe.h"
#include <stdio.h>


struct SoeCache
soe_populate(uint64_t max)
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

bool
soe_is_prime(const struct SoeCache cache, uint64_t num)
{
	dbg_assert(num < arr_size(cache));
	const bool out = (num % 2 != 0) && !arr_at_val(cache, num);
	fprintf(stderr, "%lu is a %s number\n", num, out ? "PRIME" : "composite");
	return out;
}
