
#ifndef _SPR_H
#define _SPR_H

#include "./typ.h"

// Math for accessing values at the Spiral

u64 spr_coords_to_val(i64 x, i64 y);
i64 spr_screen_to_coord_x(u64 x, u64 wh);
i64 spr_screen_to_coord_y(u64 y, u64 wh);

#endif  // _SPR_H
