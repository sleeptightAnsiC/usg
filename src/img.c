
#include "./img.h"
#include "./dbg.h"
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>


DBG_STATIC_ASSERT(UINT8_MAX == 255);

#define _img_fprintf(...) \
	do { \
		const int _result = fprintf(__VA_ARGS__); \
		dbg_assert(_result > 0); \
	} while (0) \

#define _img_fputc(...) \
	do { \
		const int _result = fputc(__VA_ARGS__); \
		dbg_assert(_result != EOF); \
	} while (0) \

#define _img_fwrite(...) \
	do { \
		const size_t _result = fwrite(__VA_ARGS__); \
		dbg_assert(_result > 0); \
	} while (0) \

struct ImgContext img_init(const char *name, uint32_t w, uint32_t h, enum ImgType t)
{
	FILE *const file = fopen(name , "w");
	// TODO: opening the file should have error handling
	dbg_assert(file);
	struct ImgContext ctx = {
		._pixels = 0,
		._file = file,
		._width = w,
		._height = h,
		._type = t,
	};
	switch (ctx._type) {
	case IMG_TYPE_PPM: {
		_img_fprintf(file, "P3\n");
		_img_fprintf(file, "%"PRIu32" %"PRIu32"\n", w, h);
		_img_fprintf(file, "255\n");
		break;
	} case IMG_TYPE_BMP: {
		// Bitmap file header - BMP Identifier - 2 bytes
		_img_fputc('B', file);
		_img_fputc('M', file);
		// Bitmap file header - The size of the BMP file in bytes - 4 bytes
		uint32_t bmpsize = 14 + 12 + (uint32_t)(w * h * sizeof(uint32_t));
		dbg_assert(bmpsize < UINT32_MAX / 2); // (on Windows this value is signed)
		_img_fwrite(&bmpsize, sizeof(bmpsize), 1, file);
		// Bitmap file header - Reserved space - 2 + 2 bytes
		_img_fputc('!', file);
		_img_fputc('u', file);
		_img_fputc('s', file);
		_img_fputc('g', file);
		// Bitmap file header - Starting adress of the pixel array - 4 bytes
		uint32_t start = 14 + 12;
		_img_fwrite(&start, sizeof(start), 1, file);
		// DIB BITMAPCOREHEADER - The size of DIB header in bytes (12) - 4 bytes
		uint32_t dibsize = 12;
		_img_fwrite(&dibsize, sizeof(dibsize), 1, file);
		// DIB BITMAPCOREHEADER - Bitmap width in pixels - 2 bytes
		dbg_assert(w <= UINT16_MAX); // FIXME: use u32 for w/h instead of validation
		uint16_t width = (uint16_t)w;
		_img_fwrite(&width, sizeof(width), 1, file);
		// DIB BITMAPCOREHEADER - Bitmap height in pixels - 2 bytes
		dbg_assert(h <= UINT16_MAX); // FIXME: use u32 for w/h instead of validation
		uint16_t height = (uint16_t)w;
		_img_fwrite(&height, sizeof(height), 1, file);
		// DIB BITMAPCOREHEADER - Number of color planes (always 1) - 2 bytes
		uint16_t planes = 1;
		_img_fwrite(&planes, sizeof(planes), 1, file);
		// DIB BITMAPCOREHEADER - Number of bits per pixel (8 bits * 4 channels) - 2 bytes
		uint16_t bits = 32;
		_img_fwrite(&bits, sizeof(bits), 1, file);
		break;
	} case IMG_TYPE_INVALID: {
	} default: {
		dbg_unreachable();
	} // end default
	} // end switch
	return ctx;
}

void img_deinit(struct ImgContext *ctx)
{
	dbg_assert(ctx != NULL);
	const int err = fclose(ctx->_file);
	// TODO: closing the file should have error handling
	dbg_assert(err == 0);
	dbg_assert(ctx->_height * ctx->_width == ctx->_pixels);
}

void img_write(struct ImgContext *ctx, struct ImgPixel px)
{
	dbg_assert(ctx != NULL);
	dbg_assert(ctx->_height * ctx->_width > ctx->_pixels);
	DBG_CODE {
		ctx->_pixels += 1;
	}
	switch (ctx->_type) {
	case IMG_TYPE_PPM:
		// FIXME: TOOOOO SLOW...
		_img_fprintf(
			ctx->_file,
			"%"PRIu8" %"PRIu8" %"PRIu8"\n",
			px.r,
			px.g,
			px.b);
		(void)px.a;
		break;
	case IMG_TYPE_BMP:
		_img_fputc(px.r, ctx->_file);
		_img_fputc(px.g, ctx->_file);
		_img_fputc(px.b, ctx->_file);
		_img_fputc(px.a, ctx->_file);
		break;
	case IMG_TYPE_INVALID:
	default:
		dbg_unreachable();
	}
}

