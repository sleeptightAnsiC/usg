
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "./img.h"
#include "./dbg.h"
#include "./typ.h"


DBG_STATIC_ASSERT(UINT8_MAX == 255);

#define _IMG_FPRINTF(...) \
	do { \
		const int __result = fprintf(__VA_ARGS__); \
		if (__result == 0) { \
			fprintf(stderr, "Unable to write to the file: %s\n", strerror(errno)); \
			exit(errno); \
		} \
	} while (0) \

// Takes VAL, stores it as TYPE (type coersion/conversion is expected)
// and writes it to FILE with fwrite. Unlike fwrite, this only takes one value.
#define _IMG_FDUMP(TYPE, VAL, FILE) \
	do { \
		TYPE __val = (TYPE)(VAL); \
		const size_t __result = fwrite(&__val, sizeof(__val), 1, (FILE)); \
		if (__result == 0) { \
			fprintf(stderr, "Unable to write to the file: %s\n", strerror(errno)); \
			exit(errno); \
		} \
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
		_IMG_FPRINTF(file, "P3\n");
		_IMG_FPRINTF(file, "%"PRIu32" %"PRIu32"\n", w, h);
		_IMG_FPRINTF(file, "255\n");
		break;
	} case IMG_TYPE_BMP: {
		const u8 HDRSIZE = 14; // size of file header
		const u8 DIBSIZE = 40; // size of DIB header
		{  // file header
			// BMP Identifier ("BM") - 2 bytes
			_IMG_FDUMP(u8, 'B', file);
			_IMG_FDUMP(u8, 'M', file);
			// The size of the BMP file in bytes (pixel array + headers) - 4 bytes
			DBG_STATIC_ASSERT(sizeof(struct img_pixel) == sizeof(u32));
			size_t bmpsize = w * h * sizeof(struct img_pixel) + HDRSIZE + DIBSIZE;
			_IMG_FDUMP(u32, bmpsize, file);
			// Reserved space (empty in this case, but can be anything) - 2 + 2 bytes
			_IMG_FDUMP(u32, 0, file);
			// Starting adress of the pixel array - 4 bytes
			_IMG_FDUMP(u32, HDRSIZE + DIBSIZE, file);
		}
		{  // DIB header BITMAPINFOHEADER
			// the size of this header, in bytes (40) - 4 bytes
			_IMG_FDUMP(u32, DIBSIZE, file);
			// the bitmap width in pixels (signed integer) - 4 bytes
			dbg_assert(w < INT32_MAX);
			_IMG_FDUMP(i32, (i32)w, file);
			// the bitmap height in pixels (signed integer) - 4 bytes
			dbg_assert(h < INT32_MAX);
			_IMG_FDUMP(i32, (i32)h, file);
			// the number of color planes (must be 1) - 2 bytes
			_IMG_FDUMP(u16, 1, file);
			// the number of bits per pixel, which is the color depth of the image - 2 bytes
			_IMG_FDUMP(u16, 32, file);
			// the compression method being used (0 for BI_RGB which stands for no compression) - 4 bytes
			_IMG_FDUMP(u32, 0, file);
			// the image size. This is the size of the raw bitmap data; a dummy 0 can be given for BI_RGB bitmaps. - 4 bytes
			_IMG_FDUMP(u32, 0, file);
			// the horizontal resolution of the image. (pixel per metre, signed integer) - 4 bytes
			// FIXME: I'm not sure what it does but stb_image sets it to 0
			_IMG_FDUMP(i32, 0, file);
			// the vertical resolution of the image. (pixel per metre, signed integer) - 4 bytes
			// FIXME: I'm not sure what it does but stb_image sets it to 0
			_IMG_FDUMP(i32, 0, file);
			// the number of colors in the color palette, or 0 to default to 2n - 4 bytes
			_IMG_FDUMP(u32, 0, file);
			// the number of important colors used, or 0 when every color is important; generally ignored - 4 bytes
			_IMG_FDUMP(u32, 0, file);
		}
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
		_IMG_FPRINTF(
			ctx->_file,
			"%"PRIu8" %"PRIu8" %"PRIu8"\n",
			px.r,
			px.g,
			px.b);
		(void)px.a;
		break;
	case IMG_TYPE_BMP:
		_IMG_FDUMP(u8, px.b, ctx->_file);
		_IMG_FDUMP(u8, px.g, ctx->_file);
		_IMG_FDUMP(u8, px.r, ctx->_file);
		_IMG_FDUMP(u8, px.a, ctx->_file);
		break;
	case IMG_TYPE_INVALID:
	default:
		dbg_unreachable();
	}
}

b8
img_pixel_from_arg(struct img_pixel *pix_out, const char *arg)
{
	// TODO: perhaps it would be just easier to use strtoumax
	// with reinterpret cast instead of parsing manually
	// https://en.cppreference.com/w/c/string/byte/strtoimax
	// NOTE: after checking strtomax out, I don't like
	// how it handles the garbage isside of string and reports errors
	// TODO: let's merge it someday with branch parsing --size argument in main()
	dbg_assert(pix_out != NULL);
	dbg_assert(arg != NULL);
	u8 vals[8];
	for (int i = 0; i < 8; i += 1) {
		DBG_STATIC_ASSERT('0' < 'A');
		DBG_STATIC_ASSERT('A' < 'a');
		if (arg[i] == '\0') return false;
		if (arg[i] < '0') return false;
		if (arg[i] > 'f') return false;
		if (arg[i] >= 'a')
			vals[i] = (u8)(arg[i] - 'a' + 10);
		else if (arg[i] >= 'A')
			vals[i] = (u8)(arg[i] - 'A' + 10);
		else if (arg[i] >= '0')
			vals[i] = (u8)(arg[i] - '0');
		else
			dbg_unreachable();
	}
	if (arg[8] != '\0') return false;
	pix_out->r = (u8)(vals[0] * 16 + vals[1]);
	pix_out->g = (u8)(vals[2] * 16 + vals[3]);
	pix_out->b = (u8)(vals[4] * 16 + vals[5]);
	pix_out->a = (u8)(vals[6] * 16 + vals[7]);
	return true;
}

