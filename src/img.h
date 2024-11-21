
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
 *	It uses 14 byte bmp header followed by 12 byte DIB BITMAPCOREHEADER header,
 *	with signed integers used for storing width and height.
 *	https://en.wikipedia.org/wiki/BMP_file_format
 *
 */

enum img_type {
	IMG_TYPE_INVALID,
	IMG_TYPE_PPM,
	IMG_TYPE_BMP,
};

// TODO: perhaps, rename to 'color' since
// img_color makes more sense that img_pixel
struct img_pixel {
	u8 r;
	u8 g;
	u8 b;
	u8 a;
};

struct img_context {
	u64 _pixels;
	FILE *const _file;
	const u32 _width;
	const u32 _height;
	const enum img_type _type;
};

struct img_context img_init(const char *name, u32 w, u32 h, enum img_type t);
void img_deinit(struct img_context *ctx);
void img_write(struct img_context *ctx, struct img_pixel px);
b8 img_pixel_from_arg(struct img_pixel *pix_out, const char *arg);

#endif  // _IMG_H
