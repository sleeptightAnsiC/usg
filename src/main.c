#include <stdint.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "./dbg.h"
#include "./soe.h"
#include "./spr.h"
#include "./img.h"

int
main(void)
{
	const uint32_t WH = 101;
	// const uint32_t WH = 4001;
	const uint64_t MAX = WH * WH;
	const struct SoeCache cache = soe_init(MAX);
	// struct ImgContext image = img_init("spiral.ppm", WH, WH, IMG_TYPE_PPM);
	struct ImgContext image = img_init("spiral.bmp", WH, WH, IMG_TYPE_BMP);
	for (uint64_t i = 0; i < WH; ++i) {
		for (uint64_t j = 0; j < WH; ++j) {
			const int64_t x = spr_screen_to_coord_x(j, WH);
			const int64_t y = spr_screen_to_coord_y(i, WH);
			const uint64_t val = spr_coords_to_val(x,y);
			const bool prime = soe_is_prime(cache, val);
			const struct ImgPixel black = {.r=0, .g=0, .b=0, .a=255};
			const struct ImgPixel white = {.r=255, .g=255, .b=255, .a=255};
			const struct ImgPixel color = prime ? black : white;
			img_write(&image, color);
		}
	}
	img_deinit(&image);
	soe_deinit(cache);
	return EXIT_SUCCESS;
}

