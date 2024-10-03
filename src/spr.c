
#include <stdint.h>
#include "./spr.h"
#include "./dbg.h"


#define _spr_abs(VAL) \
	(((VAL) < 0) ? (-(VAL)) : (VAL)) \

#define _spr_max(A, B) \
	(((A) > (B)) ? (A) : (B)) \

#define _spr_pow2(VAL) \
	((VAL) * (VAL)) \


uint64_t
spr_coords_to_val(int64_t x, int64_t y)
{
	const int64_t xabs = _spr_abs(x);
	const int64_t yabs = _spr_abs(y);
	const int64_t layer = _spr_max(xabs, yabs);
	const int64_t max = _spr_pow2(layer * 2 + 1);
	int64_t val;
	if (y == -layer) {
		val = max - (layer - x);
	} else if (x == -layer) {
		val = max - (2 * layer) - (layer + y);
	} else if (y == layer) {
		val = max - (4 * layer) - (layer + x);
	} else {
		val = max - (6 * layer) - (layer - y);
	}
	return (uint64_t)val;
}

int64_t
spr_screen_to_coord_x(uint64_t x, uint64_t wh)
{
	dbg_assert(wh % 2 != 0);
	const uint64_t half = wh / 2;
	const int64_t coord = (int64_t)(x - half);
	return coord;
}

int64_t
spr_screen_to_coord_y(uint64_t y, uint64_t wh)
{
	dbg_assert(wh % 2 != 0);
	const uint64_t half = wh / 2;
	const int64_t coord = (int64_t)(half - y);
	return coord;
}


