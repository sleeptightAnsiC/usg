#include <stdlib.h>
#include "./soe.h"
#include "./spr.h"
#include "./img.h"
#include "./typ.h"

int
main(void)
{
	const u32 WH = 101;
	// const u32 WH = 4001;
	const u64 MAX = WH * WH;
	const struct soe_cache cache = soe_init(MAX);
	// struct ImgContext image = img_init("spiral.ppm", WH, WH, IMG_TYPE_PPM);
	struct img_context image = img_init("spiral.bmp", WH, WH, IMG_TYPE_BMP);
	for (u64 i = 0; i < WH; ++i) {
		for (u64 j = 0; j < WH; ++j) {
			const i64 x = spr_screen_to_coord_x(j, WH);
			const i64 y = spr_screen_to_coord_y(i, WH);
			const u64 val = spr_coords_to_val(x,y);
			const b8 prime = soe_is_prime(cache, val);
			const struct img_pixel black = {.r=0, .g=0, .b=0, .a=255};
			const struct img_pixel white = {.r=255, .g=255, .b=255, .a=255};
			const struct img_pixel color = prime ? black : white;
			img_write(&image, color);
		}
	}
	img_deinit(&image);
	soe_deinit(cache);
	return EXIT_SUCCESS;
}

