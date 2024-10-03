
#ifndef IMG_H
#define IMG_H

#include <stdint.h>
#include <stdio.h>

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
 *	portable pixmap format - human readable, uncompressed, NOT widely suported
 *	This is the simplest image format there is, but only few programs support it.
 *	Highly inefficient - huge size, slow to write/read.
 *	Does NOT support Alpha channel.
 *	https://en.wikipedia.org/wiki/Netpbm
 *
 * .bmp
 *	Bitmap file - NOT human readable, uncompressed, widely suported
 *	This is as minimal BMP format as it is possible to implement.
 *	It uses 14 byte bmp header followed by 12 byte DIB BITMAPCOREHEADER header,
 *	with signed integers used for storing width and height.
 *	https://en.wikipedia.org/wiki/BMP_file_format
 *	https://web.archive.org/web/20080912171714/http://www.fortunecity.com/skyscraper/windows/364/bmpffrmt.html
 */

enum ImgType {
	IMG_TYPE_INVALID,
	IMG_TYPE_PPM,
	IMG_TYPE_BMP,
};

struct ImgPixel {
	uint8_t r;
	uint8_t g;
	uint8_t b;
	uint8_t a;
};

struct ImgContext {
	uint64_t _pixels;
	FILE *const _file;
	const uint32_t _width;
	const uint32_t _height;
	const enum ImgType _type;
};

struct ImgContext img_init(const char *name, uint32_t w, uint32_t h, enum ImgType t);
void img_deinit(struct ImgContext *ctx);
void img_write(struct ImgContext *ctx, struct ImgPixel px);

#endif  // IMG_H
