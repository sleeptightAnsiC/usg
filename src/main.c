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
	const uint32_t wh = 101;
	// const uint32_t wh = 4001;
	const uint64_t max = wh * wh;
	const struct SoeCache cache = soe_init(max);
	// struct ImgContext image = img_init("test.ppm", wh, wh, IMG_TYPE_PPM);
	struct ImgContext image = img_init("test.bmp", wh, wh, IMG_TYPE_BMP);
	for (uint64_t i = 0; i < wh; ++i) {
		for (uint64_t j = 0; j < wh; ++j) {
			const int64_t x = spr_screen_to_coord_x(j, wh);
			const int64_t y = spr_screen_to_coord_y(i, wh);
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

