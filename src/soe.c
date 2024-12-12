#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>  // calloc free size_t
#include "./soe.h"
#include "./dbg.h"
#include "./typ.h"


static inline void _soe_set_compisite(struct soe_cache *cache, u64 num);
static inline b8 _soe_is_composite(const struct soe_cache *cache, u64 num);


b8
soe_init(struct soe_cache *cache, u64 max)
{
	dbg_assert(cache != NULL);
	dbg_assert(max >= 1);
	dbg_assert(max <= UINT64_MAX - 1);
	max += 1;

	// HACK: allocate mem for soe_cache and soe_cache._data in one calloc call
	// this is stupid and unnesesairy but I really wanted to try it out...
	const u64 cap = (max / 16) + 1;
	void *const data = calloc(cap, sizeof(u8));
	if (data == NULL) return false;
	cache->_cap = cap;
	cache->_data = data;

	_soe_set_compisite(cache, 1);
	for (u64 i = 3; (i * i) < max; i += 2) {
		if (_soe_is_composite(cache, i))
			continue;
		for (u64 p = i * 3; p < max; p += i * 2)
			_soe_set_compisite(cache, p);
	}

	return true;
}

b8
soe_deinit(struct soe_cache *cache)
{
	dbg_assert(cache != NULL);
#	ifndef DBG_DISABLED
		const size_t mem = sizeof(cache->_data[0]) * cache->_cap;
		const f64 fmt = (f64)(mem) / 1024 / 1024 ;
		dbg_log("Total heap size allocation for soe_cache._data : %f MiB", fmt);
		(void)(fmt);
#	endif
	free(cache->_data);
	return true;
}

b8
soe_is_prime(const struct soe_cache *cache, u64 num)
{
	dbg_assert(num > 0);
	return (num == 2) || ((num % 2 != 0) && !_soe_is_composite(cache, num));
}

static inline void
_soe_set_compisite(struct soe_cache *cache, u64 num)
{
	dbg_assert(num <= cache->_cap * 8 * 2);
	dbg_assert(num % 2 != 0);
	const u64 idx = num / (8 * 2);
	const u8 whole = cache->_data[idx];
	const u8 mask = (uint8_t)(1 << ((num / 2) % 8));
	const u8 new = whole | mask;
	cache->_data[idx] = new;
}

static inline b8
_soe_is_composite(const struct soe_cache *cache, u64 num)
{
	dbg_assert(num <= cache->_cap * 8 * 2);
	dbg_assert(num % 2 != 0);
	const u64 idx = num / (8 * 2);
	const u8 whole = cache->_data[idx];
	const u8 mask = (uint8_t)(whole >> ((num / 2) % 8));
	const u8 actual = 1 & mask;
	return actual;
}

