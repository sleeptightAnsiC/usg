
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "./img.h"
#include "./dbg.h"
#include "./ccl.h"
#include "./typ.h"


#define _IMG_FPRINTF(...) \
	do { \
		const int __result = fprintf(__VA_ARGS__); \
		dbg_assert(__result > 0); \
		(void)__result; \
	} while (0) \

// WARN: _IMG_FDUMP supports only systems with Little and Big Endians
DBG_STATIC_ASSERT(CCL_ENDIAN_ORDER == CCL_ENDIAN_LITTLE || CCL_ENDIAN_ORDER == CCL_ENDIAN_BIG);

// Dumps value into the file with fwrite
// this macro also flips bytes in case file requires Endian different than the one being used
#define _IMG_FDUMP(TYPE, VAL, FILE, ENDIANESS) \
	do { \
		CCL_PRAGMA("GCC diagnostic push"); \
		CCL_PRAGMA("GCC diagnostic ignored \"-Wuseless-cast\""); \
		TYPE val__ = (TYPE)(VAL); \
		if (CCL_ENDIAN_ORDER != (ENDIANESS)) { \
			TYPE out__ = 0; \
			for (size_t i__ = 0; i__ < sizeof(TYPE); ++i__) \
				out__ |= (TYPE)((val__ >> (i__ * 8)) & 0xFF) << ((sizeof(TYPE) - 1 - i__) * 8); \
			val__ = out__; \
		} \
		const size_t result__ = fwrite(&val__, sizeof(val__), 1, (FILE)); \
		dbg_assert(result__ > 0); \
		(void)result__; \
		CCL_PRAGMA("GCC diagnostic pop"); \
	} while (0) \

// Dumps value into the file ensuring that Little Endian is being used
#define _IMG_FDUMP_LE(TYPE, VAL, FILE) \
	_IMG_FDUMP(TYPE, VAL, FILE, CCL_ENDIAN_LITTLE) \

// Dumps value into the file ensuring that Big Endian is being used
#define _IMG_FDUMP_BE(TYPE, VAL, FILE) \
	_IMG_FDUMP(TYPE, VAL, FILE, CCL_ENDIAN_BIG) \

#define _img_max(A, B) \
	((A) > (B) ? (A) : (B))

#define _img_abs(VAL) \
	((VAL) < 0 ? (-VAL) : (VAL))

#define _img_pow2(VAL) \
	((VAL) * (VAL))


void
img_write_header(struct img_context *ctx)
{
	dbg_assert(ctx != NULL);
	switch (ctx->type) {
	case IMG_TYPE_PPM: {
		// .ppm header starts with "P3" indentifier on the 1st line
		// followed by width and height (with space between) on the 2nd line
		// and max color value (255) on the 3rd line
		_IMG_FPRINTF(ctx->file, "P3\n");
		_IMG_FPRINTF(ctx->file, "%"PRIu32" %"PRIu32"\n", ctx->width, ctx->height);
		_IMG_FPRINTF(ctx->file, "255\n");
		break;
	} case IMG_TYPE_BMP: {
		const u8 HDRSIZE = 14; // size of file header
		const u8 DIBSIZE = 40; // size of DIB header
		{  // file header
			// BMP Identifier ("BM") - 2 bytes
			_IMG_FDUMP_LE(u8, 'B', ctx->file);
			_IMG_FDUMP_LE(u8, 'M', ctx->file);
			// The size of the BMP file in bytes (pixel array + headers) - 4 bytes
			DBG_STATIC_ASSERT(sizeof(struct img_color) == sizeof(u32));
			size_t bmpsize = ctx->width * ctx->height * sizeof(struct img_color) + HDRSIZE + DIBSIZE;
			_IMG_FDUMP_LE(u32, bmpsize, ctx->file);
			// Reserved space (empty in this case, but can be anything) - 2 + 2 bytes
			_IMG_FDUMP_LE(u32, 0, ctx->file);
			// Starting adress of the pixel array - 4 bytes
			_IMG_FDUMP_LE(u32, HDRSIZE + DIBSIZE, ctx->file);
		}
		{  // DIB header BITMAPINFOHEADER
			// the size of this header, in bytes (40) - 4 bytes
			_IMG_FDUMP_LE(u32, DIBSIZE, ctx->file);
			// the bitmap width in pixels (signed integer) - 4 bytes
			dbg_assert(ctx->width < INT32_MAX);
			_IMG_FDUMP_LE(i32, (i32)(ctx->width), ctx->file);
			// the bitmap height in pixels (signed integer) - 4 bytes
			dbg_assert(ctx->height < INT32_MAX);
			_IMG_FDUMP_LE(i32, (i32)(ctx->height), ctx->file);
			// the number of color planes (must be 1) - 2 bytes
			_IMG_FDUMP_LE(u16, 1, ctx->file);
			// the number of bits per pixel, which is the color depth of the image - 2 bytes
			_IMG_FDUMP_LE(u16, 32, ctx->file);
			// the compression method being used (0 for BI_RGB which stands for no compression) - 4 bytes
			_IMG_FDUMP_LE(u32, 0, ctx->file);
			// the image size. This is the size of the raw bitmap data; a dummy 0 can be given for BI_RGB bitmaps. - 4 bytes
			_IMG_FDUMP_LE(u32, 0, ctx->file);
			// FIXME: I'm not sure what the next two do but checked in stb_image and it sets them to 0
			// the horizontal resolution of the image. (pixel per metre, signed integer) - 4 bytes
			_IMG_FDUMP_LE(i32, 0, ctx->file);
			// the vertical resolution of the image. (pixel per metre, signed integer) - 4 bytes
			_IMG_FDUMP_LE(i32, 0, ctx->file);
			// the number of colors in the color palette, or 0 to default to 2n - 4 bytes
			_IMG_FDUMP_LE(u32, 0, ctx->file);
			// the number of important colors used, or 0 when every color is important; generally ignored - 4 bytes
			_IMG_FDUMP_LE(u32, 0, ctx->file);
		}
		break;
	} case IMG_TYPE_INVALID: {
	} default: {
		dbg_unreachable();
	} // end default
	} // end switch
}

void
img_write_pixel(struct img_context *ctx, struct img_color col)
{
	dbg_assert(ctx != NULL);
	switch (ctx->type) {
	case IMG_TYPE_PPM: {
		_IMG_FPRINTF(
			ctx->file,
			"%"PRIu8" %"PRIu8" %"PRIu8"\n",
			col.r,
			col.g,
			col.b);
		(void)col.a;
		break;
	} case IMG_TYPE_BMP: {
		_IMG_FDUMP_LE(u8, col.b, ctx->file);
		_IMG_FDUMP_LE(u8, col.g, ctx->file);
		_IMG_FDUMP_LE(u8, col.r, ctx->file);
		_IMG_FDUMP_LE(u8, col.a, ctx->file);
		break;
	} case IMG_TYPE_INVALID: {
	} default:
		dbg_unreachable();
	}
}

u64
img_val_from_coords(struct img_context *ctx, u32 x, u32 y)
{
	dbg_assert(ctx != NULL);
	dbg_assert(x < ctx->width);
	dbg_assert(y < ctx->height);

	// NOTE: BMP stores images bottom-to-top by default.
	// said format supports flipping the image by setting the height
	// to negative value but after testing this out it seems image viewers
	// do not handle it very well - e.g. some may flip the image as expected
	// but still display negative height. I don't wanna relay on this and
	// prefer to flip the image manually.
	if (ctx->type == IMG_TYPE_BMP)
		y = ctx->height - y - 1;

	const i64 adj_x = (i64)x - (i64)(ctx->start_x);
	const i64 adj_y = (i64)(ctx->start_y) - (i64)y;
	const i64 adj_x_abs = _img_abs(adj_x);
	const i64 adj_y_abs = _img_abs(adj_y);
	const i64 layer = _img_max(adj_x_abs, adj_y_abs);
	const i64 max = _img_pow2(layer * 2 + 1);

	i64 val;
	if (adj_y == -layer) {
		val = max - (layer - adj_x);
	} else if (adj_x == -layer) {
		val = max - (2 * layer) - (layer + adj_y);
	} else if (adj_y == layer) {
		val = max - (4 * layer) - (layer + adj_x);
	} else if (adj_x == layer) {
		val = max - (6 * layer) - (layer - adj_y);
	} else {
		dbg_unreachable();
	}

	dbg_assert(val >= 0);
	dbg_assert((u64)val <= UINT64_MAX - ctx->start_val + 1);
	const u64 out = (u64)val + ctx->start_val - 1;
	return out;
}

u64
img_val_max(struct img_context *ctx)
{
	dbg_assert(ctx != NULL);
	dbg_assert(ctx->width <= UINT64_MAX / ctx->height);
	dbg_assert(ctx->height <= UINT64_MAX / ctx->width);
	// FIXME: the math for adj_width/height is way too ugly...
	const u32 adj_width = (ctx->start_x < ctx->width / 2) ? ((ctx->width - ctx->start_x) * 2 + 1) : (ctx->start_x * 2 + 1);
	const u32 adj_height = (ctx->start_y < ctx->height / 2) ? ((ctx->height - ctx->start_y) * 2 + 1) : (ctx->start_y * 2 + 1);
	const u32 max = _img_max(adj_width, adj_height);
	const u64 out = _img_pow2(max) + ctx->start_val;
	return out;
}

struct img_color
img_color_faded(struct img_color a, struct img_color b, f32 ratio)
{
	const struct img_color diff = {
		.r = (a.r > b.r) ? (a.r - b.r) : (b.r - a.r),
		.g = (a.g > b.g) ? (a.g - b.g) : (b.g - a.g),
		.b = (a.b > b.b) ? (a.b - b.b) : (b.b - a.b),
		.a = (a.a > b.a) ? (a.a - b.a) : (b.a - a.a),
	};
	const struct img_color part = {
		.r = (u8)(ratio * (f32)diff.r),
		.g = (u8)(ratio * (f32)diff.g),
		.b = (u8)(ratio * (f32)diff.b),
		.a = (u8)(ratio * (f32)diff.a),
	};
	const struct img_color out = (struct img_color){
		.r = (a.r > b.r) ? (part.r + b.r) : (a.r + part.r),
		.g = (a.g > b.g) ? (part.g + b.g) : (a.g + part.g),
		.b = (a.b > b.b) ? (part.b + b.b) : (a.b + part.b),
		.a = (a.a > b.a) ? (part.a + b.a) : (a.a + part.a),
	};
	return out;
}

b8
img_color_from_str(struct img_color *out, const char *str)
{
	dbg_assert(str != NULL);
	dbg_assert(out != NULL);
	u8 vals[8];
	for (int i = 0; i < 8; i += 1) {
		DBG_STATIC_ASSERT('0' < 'A');
		DBG_STATIC_ASSERT('A' < 'a');
		if (str[i] == '\0') return false;
		if (str[i] < '0') return false;
		if (str[i] > 'f') return false;
		if (str[i] >= 'a')
			vals[i] = (u8)(str[i] - 'a' + 10);
		else if (str[i] >= 'A')
			vals[i] = (u8)(str[i] - 'A' + 10);
		else if (str[i] >= '0')
			vals[i] = (u8)(str[i] - '0');
		else
			dbg_unreachable();
	}
	if (str[8] != '\0') return false;
	out->r = (u8)(vals[0] * 16 + vals[1]);
	out->g = (u8)(vals[2] * 16 + vals[3]);
	out->b = (u8)(vals[4] * 16 + vals[5]);
	out->a = (u8)(vals[6] * 16 + vals[7]);
	return true;
}

