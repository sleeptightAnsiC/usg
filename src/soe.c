#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "./soe.h"
#include "./dbg.h"


#define SOE_OPTIMIZED_MEM

static inline void _soe_composite_set(struct SoeCache cache, uint64_t num);
static inline bool _soe_is_composite(struct SoeCache cache, uint64_t num);


static inline void
_soe_composite_set(struct SoeCache cache, uint64_t num)
{
#	ifdef SOE_OPTIMIZED_MEM
		dbg_assert(num <= cache._max * 8 * 2);
		dbg_assert(num % 2 != 0);
		const uint64_t idx = num / (8 * 2);
		const uint8_t whole = cache._data[idx];
		const uint8_t mask = (uint8_t)(1 << ((num / 2) % 8));
		const uint8_t new = whole | mask;
		cache._data[idx] = new;
#	else
		dbg_assert(num < cache._max);
		cache._data[num] = 1;
#	endif
}

static inline bool
_soe_is_composite(struct SoeCache cache, uint64_t num)
{
#	ifdef SOE_OPTIMIZED_MEM
		dbg_assert(num <= cache._max * 8 * 2);
		dbg_assert(num % 2 != 0);
		if (num % 2 == 0)
			return true;
		const uint64_t idx = num / (8 * 2);
		const uint8_t whole = cache._data[idx];
		const uint8_t mask = (uint8_t)(whole >> ((num / 2) % 8));
		const uint8_t actual = 1 & mask;
		return actual;
#	else
		dbg_assert(num < cache._max);
		return cache._data[num];
#	endif
}

struct SoeCache
soe_init(uint64_t max)
{
	dbg_assert(max >= 1);
	dbg_assert(max <= UINT64_MAX - 1);
	struct SoeCache out;
	max += 1;
#	ifdef SOE_OPTIMIZED_MEM
		out._max = max / 16;
		out._data = calloc((size_t)(out._max), sizeof(out._data[0]));
		dbg_assert(out._data != NULL);
		_soe_composite_set(out, 1);
		for (uint64_t i = 3; (i * i) < max; i += 2) {
			if (_soe_is_composite(out, i))
				continue;
			for (uint64_t p = i * 3; p < max; p += i * 2)
				_soe_composite_set(out, p);
		}
#	else
		out._max = max;
		out._data = calloc((size_t)(out._max), sizeof(out._data[0]));
		_soe_composite_set(out, 1);
		for (uint64_t i = 2; (i * i) < max; ++i) {
			if (_soe_is_composite(out, i))
				continue;
			for (uint64_t p = i * 2; p < max; p += i)
				_soe_composite_set(out, p);
		}
#	endif
		return out;
}

void
soe_deinit(const struct SoeCache cache)
{
	DBG_CODE {
		const size_t mem = sizeof(cache._data[0]) * cache._max;
		const double fmt = (double)(mem) / 1024 / 1024 ;
		fprintf(stderr, "[soe] Total cache allocation: %.1f MiB \n", fmt);
	}
	free(cache._data);
}

bool
soe_is_prime(const struct SoeCache cache, uint64_t num)
{
	bool out;
	dbg_assert(num > 0);
#	ifdef SOE_OPTIMIZED_MEM
		out = (num == 2) || ((num % 2 != 0) && !_soe_is_composite(cache, num));
#	else
		out = !_soe_is_composite(cache, num);
#	endif
	return out;
}

