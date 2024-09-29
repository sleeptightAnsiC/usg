#include "./soe.h"
#include "arr.h"
#include <stdint.h>
#include <stdio.h>


void _soe_composite_set(struct SoeCache cache, uint64_t num)
{
	dbg_assert(num <= arr_size(cache) * 16);
	if (num % 2 == 0)
		return;
	const uint64_t idx = num / 16;
	const uint8_t current = arr_at(cache, idx);
	const uint8_t mask = (uint8_t)(1 << ((num / 2) % 8));
	const uint8_t new = current | mask;
	arr_at(cache, idx) = new;
}

bool _soe_is_composite(struct SoeCache cache, uint64_t num)
{
	dbg_assert(num <= arr_size(cache) * 16);
	if (num % 2 == 0)
		return true;
	const uint64_t idx = num / 16;
	const uint8_t whole = arr_at(cache, idx);
	const uint8_t actual = (whole >> ((num / 2) % 8)) & 1;
	return actual;
}

struct SoeCache
soe_init(uint64_t max)
{
	dbg_assert(max >= 1);
	dbg_assert(max <= UINT64_MAX - 1);
	struct SoeCache out;
	max += 1;
#ifdef SOE_OPTIMIZE_MEM
	arr_init_null(out, max / 16);
	_soe_composite_set(out, 1);
	for (uint64_t i = 3; (i * i) < max; i += 2) {
		if (_soe_is_composite(out, i))
			continue;
		for (uint64_t p = i * 3; p < max; p += i * 2)
			_soe_composite_set(out, p);
	}
#else
	arr_init_null(out, max);
	arr_at(out, 1) = true;
	for (uint64_t i = 2; (i * i) < max; ++i) {
		const bool new = arr_at(out, i);
		if (new)
			continue;
		for (uint64_t p = i * 2; p < max; p += i) {
			arr_at(out, p) = true;
		}
	}
#endif
	return out;
}

void
soe_deinit(const struct SoeCache cache)
{
	DBG_CODE {
		const size_t mem = arr_data_size(cache);
		const double fmt = (double)(mem) / 1024 / 1024 ;
		fprintf(stderr, "[soe] Total cache allocation: %.1f MiB \n", fmt);
	}
	arr_deinit(cache);
}

bool
soe_is_prime(const struct SoeCache cache, uint64_t num)
{
	bool out;
	dbg_assert(num >= 1);
#	ifdef SOE_OPTIMIZE_MEM
		out = !_soe_is_composite(cache, num);
#	else
		dbg_assert(num < arr_size(cache));
		out = !arr_at(cache, num);
#	endif
	DBG_CODE {
		const char *kind = out ? "PRIME" : "composite";
		fprintf(stderr, "[soe] %lu is a %s number\n", num, kind);
	}
	return out;
}
