
#ifndef _IMG_H
#define _IMG_H

#include <stdio.h>
#include "./typ.h"

/* IMG
 *
 * Simple abstraction for creating image files from 32 bit RGBA pixel stream.
 * Create image with img_init, then fill it with pixels via img_write.
 * Pixels are expected to be filled from left-to-right, top-to-bottom.
 * The amount of pixels must be the same as (width * height)
 * One must make sure to call img_deinit afterwards.
 * Not following said rules results in undefined behavior,
 * but asserts should catch it in debug builds.
 *
 * .ppm
 *	portable pixmap format - human readable, uncompressed, NOT widely supported
 *	This is the simplest image format there is, but only few programs support it.
 *	Highly inefficient - huge size, slow to write/read.
 *	Does NOT support Alpha channel.
 *	https://en.wikipedia.org/wiki/Netpbm
 *
 * .bmp
 *	Bitmap file - NOT human readable, uncompressed, widely supported
 *	This is as minimal BMP format as it is possible to implement.
 *	It uses 14 byte file header followed by 40 byte DIB BITMAPINFOHEADER
 *	https://en.wikipedia.org/wiki/BMP_file_format
 *
 */

// TODO: add .png support someday
// https://en.wikipedia.org/wiki/PNG
// http://www.libpng.org/pub/png/spec/1.2/PNG-Chunks.html
// https://www.w3.org/TR/png/#D-CRCAppendix

enum img_type {
	IMG_TYPE_INVALID,
	IMG_TYPE_PPM,
	IMG_TYPE_BMP,
	IMG_TYPE_PNG,
};

struct img_color {
	u8 r;
	u8 g;
	u8 b;
	u8 a;
};

struct img_context {
	u64 _pixels;
	FILE *_file;
	u32 _width;
	u32 _height;
	u32 _start_x;
	u32 _start_y;
	u32 _start_val;
	u32 _png_crc;
	enum img_type _type;
};

b8 img_init(struct img_context *ctx, const char *name, u32 width, u32 height, u32 start_x, u32 start_y, u32 start_val, enum img_type type);
b8 img_deinit(struct img_context *ctx);
void img_write(struct img_context *ctx, struct img_color col);
u64 img_val_from_coords(struct img_context *ctx, u32 x, u32 y);
u64 img_val_max(struct img_context *ctx);
struct img_color img_color_faded(struct img_color a, struct img_color b, f32 ratio);
b8 img_color_from_str(struct img_color *out, const char *str);

#endif  // _IMG_H
