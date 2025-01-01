
#ifndef _IMG_H
#define _IMG_H

#include <stdio.h>
#include "./typ.h"

/* IMG
 *
 * Simple abstraction for creating image files from 32 bit RGBA pixel stream.
 * Create img_context, then use img_write_header once
 * and fill image with pixels via img_write_pixel.
 * Pixels are expected to be filled from left-to-right, top-to-bottom.
 * The amount of pixels must be the same as (width * height)
 */

// TODO: currently this unit deals with 2 problems:
// creating an image and calculating specific values for the Ulam Spiral
// This is quite good because those problems partially overlap
// but since img.c becomes really big, it would be nice to split them.

enum img_type {
	IMG_TYPE_INVALID,
	IMG_TYPE_PPM,
	IMG_TYPE_BMP,
};

struct img_color {
	u8 r;
	u8 g;
	u8 b;
	u8 a;
};

struct img_context {
	FILE *file;
	u32 width;
	u32 height;
	u32 start_x;
	u32 start_y;
	u32 start_val;
	enum img_type type;
};


void img_write_header(struct img_context *ctx);
void img_write_pixel(struct img_context *ctx, struct img_color col);
u64 img_val_from_coords(struct img_context *ctx, u32 x, u32 y);
u64 img_val_max(struct img_context *ctx);
struct img_color img_color_faded(struct img_color a, struct img_color b, f32 ratio);
b8 img_color_from_str(struct img_color *out, const char *str);


#endif  // _IMG_H
