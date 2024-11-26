
#include "./spr.h"
#include "./dbg.h"
#include "./typ.h"


#define _spr_abs(VAL) \
	(((VAL) < 0) ? (-(VAL)) : (VAL)) \

#define _spr_max(A, B) \
	(((A) > (B)) ? (A) : (B)) \

#define _spr_pow2(VAL) \
	((VAL) * (VAL)) \


u64
spr_coords_to_val(i64 x, i64 y)
{
	const i64 xabs = _spr_abs(x);
	const i64 yabs = _spr_abs(y);
	const i64 layer = _spr_max(xabs, yabs);
	const i64 max = _spr_pow2(layer * 2 + 1);
	i64 val;
	if (y == -layer) {
		val = max - (layer - x);
	} else if (x == -layer) {
		val = max - (2 * layer) - (layer + y);
	} else if (y == layer) {
		val = max - (4 * layer) - (layer + x);
	} else {
		val = max - (6 * layer) - (layer - y);
	}
	return (u64)val;
}

i64
spr_screen_to_coord_x(u64 x, u64 wh)
{
	dbg_assert(wh % 2 != 0);
	const u64 half = wh / 2;
	const i64 coord = (i64)(x - half);
	return coord;
}

i64
spr_screen_to_coord_y(u64 y, u64 wh)
{
	dbg_assert(wh % 2 != 0);
	const u64 half = wh / 2;
	const i64 coord = (i64)(half - y);
	return coord;
}


