
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "./img.h"
#include "./dbg.h"
#include "./typ.h"


DBG_STATIC_ASSERT(UINT8_MAX == 255);

#define _img_fprintf(...) \
	do { \
		const int _result = fprintf(__VA_ARGS__); \
		dbg_assert(_result > 0); \
	} while (0) \

// Takes VAL, stores it as TYPE (type coersion/conversion is expected)
// and writes it to FILE with fwrite. Unlike fwrite, this only takes one value.
#define _IMG_FDUMP(TYPE, VAL, FILE) \
	do { \
		TYPE _val = (TYPE)(VAL); \
		size_t _result = fwrite(&_val, sizeof(_val), 1, (FILE)); \
		dbg_assert(_result > 0); \
	} while (0) \


struct img_context
img_init(const char *name, u32 w, u32 h, enum img_type t)
{
	dbg_log("Opening file: %s", name);
	FILE *const file = fopen(name , "w");
	if (file == NULL) {
		fprintf(stderr, "Unable to open '%s': %s\n", name, strerror(errno));
		exit(errno);
	}
	struct img_context ctx = {
		._pixels = 0,
		._file = file,
		._width = w,
		._height = h,
		._type = t,
	};
	switch (ctx._type) {
	case IMG_TYPE_PPM: {
		// .ppm header starts with "P3" indentifier on the 1st line
		// followed by width and height (with space between) on the 2nd line
		// and max color value (255) on the 3rd line
		_img_fprintf(file, "P3\n");
		_img_fprintf(file, "%"PRIu32" %"PRIu32"\n", w, h);
		_img_fprintf(file, "255\n");
		break;
	} case IMG_TYPE_BMP: {
		const u8 HDRSIZE = 14; // size of primary .bmp header
		const u8 DIBSIZE = 12; // size of DIB, secondary .bmp header
		// primary header - BMP Identifier ("BM") - 2 bytes
		_IMG_FDUMP(u8, 'B', file);
		_IMG_FDUMP(u8, 'M', file);
		// primary header - The size of the BMP file in bytes (pixel array + headers) - 4 bytes
		size_t bmpsize = w * h * sizeof(u32) + HDRSIZE + DIBSIZE;
		_IMG_FDUMP(u32, bmpsize, file);
		// primary header - Reserved space (empty in this case, but can be anything) - 2 + 2 bytes
		_IMG_FDUMP(u32, 0, file);
		// primary header - Starting adress of the pixel array - 4 bytes
		_IMG_FDUMP(u32, HDRSIZE + DIBSIZE, file);
		// DIB BITMAPCOREHEADER - The size of DIB header in bytes (12) - 4 bytes
		_IMG_FDUMP(u32, DIBSIZE, file);
		// DIB BITMAPCOREHEADER - Bitmap width in pixels - 2 bytes
		dbg_assert(w <= INT16_MAX);
		_IMG_FDUMP(i16, w, file);
		// DIB BITMAPCOREHEADER - Bitmap height in pixels - 2 bytes
		// WARN: height is negative, because image is stored from top to bottom
		dbg_assert(h <= INT16_MAX);
		_IMG_FDUMP(i16, -(i16)(h), file);
		// DIB BITMAPCOREHEADER - Number of color planes (always 1) - 2 bytes
		_IMG_FDUMP(u16, 1, file);
		// DIB BITMAPCOREHEADER - Number of bits per pixel (8 bits * 4 channels) - 2 bytes
		_IMG_FDUMP(u16, 32, file);
		break;
	} case IMG_TYPE_INVALID: {
	} default: {
		dbg_unreachable();
	} // end default
	} // end switch
	return ctx;
}

void
img_deinit(struct img_context *ctx)
{
	dbg_assert(ctx != NULL);
	const int err = fclose(ctx->_file);
	if (err != 0) {
		fprintf(stderr, "Unable to close file: %s\n", strerror(errno));
		exit(errno);
	}
	dbg_assert(ctx->_height * ctx->_width == ctx->_pixels);
	dbg_log("File closed.");
}

void
img_write(struct img_context *ctx, struct img_pixel px)
{
	dbg_assert(ctx != NULL);
	dbg_assert(ctx->_height * ctx->_width > ctx->_pixels);
	DBG_CODE {
		ctx->_pixels += 1;
	}
	switch (ctx->_type) {
	case IMG_TYPE_PPM:
		_img_fprintf(
			ctx->_file,
			"%"PRIu8" %"PRIu8" %"PRIu8"\n",
			px.r,
			px.g,
			px.b);
		(void)px.a;
		break;
	case IMG_TYPE_BMP:
		_IMG_FDUMP(u8, px.r, ctx->_file);
		_IMG_FDUMP(u8, px.g, ctx->_file);
		_IMG_FDUMP(u8, px.b, ctx->_file);
		_IMG_FDUMP(u8, px.a, ctx->_file);
		break;
	case IMG_TYPE_INVALID:
	default:
		dbg_unreachable();
	}
}

