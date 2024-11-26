#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "./soe.h"
#include "./dbg.h"
#include "./typ.h"


static inline void _soe_composite_set(struct soe_cache cache, u64 num);
static inline b8 _soe_is_composite(struct soe_cache cache, u64 num);


void
soe_deinit(const struct soe_cache cache)
{
	DBG_CODE {
		const size_t mem = sizeof(cache._data[0]) * cache._max;
		const double fmt = (double)(mem) / 1024 / 1024 ;
		dbg_log("Total heap size allocation for soe_cash::_data : %.1f MiB", fmt);
	}
	free(cache._data);
}

b8
soe_is_prime(const struct soe_cache cache, u64 num)
{
	dbg_assert(num > 0);
	return (num == 2) || ((num % 2 != 0) && !_soe_is_composite(cache, num));
}

static inline void
_soe_composite_set(struct soe_cache cache, u64 num)
{
	dbg_assert(num <= cache._max * 8 * 2);
	dbg_assert(num % 2 != 0);
	const u64 idx = num / (8 * 2);
	const u8 whole = cache._data[idx];
	const u8 mask = (uint8_t)(1 << ((num / 2) % 8));
	const u8 new = whole | mask;
	cache._data[idx] = new;
}

static inline b8
_soe_is_composite(struct soe_cache cache, u64 num)
{
	// NOTE: (8 * 2)
	// because every even number is skipped
	// and because bits are stored inside of uint8_t
	dbg_assert(num <= cache._max * 8 * 2);
	dbg_assert(num % 2 != 0);
	const u64 idx = num / (8 * 2);
	const u8 whole = cache._data[idx];
	const u8 mask = (uint8_t)(whole >> ((num / 2) % 8));
	const u8 actual = 1 & mask;
	return actual;
}

struct soe_cache
soe_init(u64 max)
{
	dbg_assert(max >= 1);
	dbg_assert(max <= UINT64_MAX - 1);
	struct soe_cache cache;
	max += 1;
	cache._max = (max / 16) + 1;
	cache._data = calloc((size_t)(cache._max), sizeof(cache._data[0]));
	dbg_assert(cache._data != NULL);
	_soe_composite_set(cache, 1);
	for (u64 i = 3; (i * i) < max; i += 2) {
		if (_soe_is_composite(cache, i))
			continue;
		for (u64 p = i * 3; p < max; p += i * 2)
			_soe_composite_set(cache, p);
	}
	return cache;
}

