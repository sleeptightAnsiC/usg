
#include "./uls.h"
#include "./dbg.h"

struct UlsContext uls_init(uint32_t start, uint32_t radius)
{
	dbg_assert(radius % 2 == 1);
	struct UlsContext out;
	out._start = start;
	out._radius = radius;
	return out;
}

// uint64_t uls_value_at_coordinate(struct UlsContext ctx, uint32_t x, uint32_t y)
// {
// 	dbg_assert(x < ctx._radius);
// 	dbg_assert(y < ctx._radius);
// }
