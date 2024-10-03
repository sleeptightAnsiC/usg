
#ifndef SPR_H
#define SPR_H

#include <stdint.h>

// Math for accessing values at the Spiral

uint64_t spr_coords_to_val(int64_t x, int64_t y);
int64_t spr_screen_to_coord_x(uint64_t x, uint64_t wh);
int64_t spr_screen_to_coord_y(uint64_t y, uint64_t wh);

#endif  // SPR_H
