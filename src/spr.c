
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
	int64_t out;
	if (y == -layer) {
		out = max - (layer - x);
	} else if (x == -layer) {
		out = max - (2 * layer) - (layer + y);
	} else if (y == layer) {
		out = max - (4 * layer) - (layer + x);
	} else {
		out = max - (6 * layer) - (layer - y);
	}
	dbg_log("(%ld:%ld) = %ld", x, y, out);
	return (uint64_t)out;
}
