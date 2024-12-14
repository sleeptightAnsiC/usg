
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "./img.h"
#include "./dbg.h"
#include "./ccl.h"
#include "./typ.h"


// WARN: this code supports only systems with Little and Big Endians
// _IMG_FDUMP_ENDIAN will break on something like PDP Endian byte order!
// Some image types require Little and some Big Endian so we need to ensure this.
DBG_STATIC_ASSERT(CCL_ENDIAN_ORDER == CCL_ENDIAN_LITTLE || CCL_ENDIAN_ORDER == CCL_ENDIAN_BIG);


#define _IMG_FPRINTF(...) \
	do { \
		const int __result = fprintf(__VA_ARGS__); \
		dbg_assert(__result > 0); \
		(void)__result; \
	} while (0) \

#define _IMG_FDUMP_ENDIAN(TYPE, VAL, FILE, WHAT_ENDIAN) \
	do { \
		CCL_PRAGMA("GCC diagnostic push"); \
		CCL_PRAGMA("GCC diagnostic ignored \"-Wuseless-cast\""); \
		TYPE val__ = (TYPE)(VAL); \
		if (CCL_ENDIAN_ORDER != (WHAT_ENDIAN)) { \
			TYPE out__ = 0; \
			u8 *reinterpret__ = (u8 *)&val__; \
			for (size_t i__ = 0; i__ < sizeof(TYPE); ++i__) { \
				out__ |= (TYPE)reinterpret__[i__] << (sizeof(TYPE) - 1) * 8; \
			} \
			val__ = out__; \
		} \
		const size_t result__ = fwrite(&val__, sizeof(val__), 1, (FILE)); \
		dbg_assert(result__ > 0); \
		(void)result__; \
		CCL_PRAGMA("GCC diagnostic pop"); \
	} while (0) \

#define _IMG_FDUMP_LE(TYPE, VAL, FILE) \
	_IMG_FDUMP_ENDIAN(TYPE, VAL, FILE, CCL_ENDIAN_LITTLE) \

#define _IMG_FDUMP_BE(TYPE, VAL, FILE) \
	_IMG_FDUMP_ENDIAN(TYPE, VAL, FILE, CCL_ENDIAN_BIG) \

#define _img_max(A, B) \
	((A) > (B) ? (A) : (B))

#define _img_abs(VAL) \
	((VAL) < 0 ? (-VAL) : (VAL))

#define _img_pow2(VAL) \
	((VAL) * (VAL))


b8
img_init(struct img_context *ctx, const char *name, u32 width, u32 height, u32 start_x, u32 start_y, u32 start_val, enum img_type type)
{
	dbg_log("Opening file: %s", name);
	FILE *const file = fopen(name , "w");
	if (file == NULL) return false;
	*ctx = (struct img_context){
		._pixels = 0,
		._file = file,
		._width = width,
		._height = height,
		._type = type,
		._start_x = start_x,
		._start_y = start_y,
		._start_val = start_val,
	};
	switch (type) {
	case IMG_TYPE_PPM: {
		// .ppm header starts with "P3" indentifier on the 1st line
		// followed by width and height (with space between) on the 2nd line
		// and max color value (255) on the 3rd line
		_IMG_FPRINTF(file, "P3\n");
		_IMG_FPRINTF(file, "%"PRIu32" %"PRIu32"\n", width, height);
		_IMG_FPRINTF(file, "255\n");
		break;
	} case IMG_TYPE_BMP: {
		const u8 HDRSIZE = 14; // size of file header
		const u8 DIBSIZE = 40; // size of DIB header
		{  // file header
			// BMP Identifier ("BM") - 2 bytes
			_IMG_FDUMP_LE(u8, 'B', file);
			_IMG_FDUMP_LE(u8, 'M', file);
			// The size of the BMP file in bytes (pixel array + headers) - 4 bytes
			DBG_STATIC_ASSERT(sizeof(struct img_color) == sizeof(u32));
			size_t bmpsize = width * height * sizeof(struct img_color) + HDRSIZE + DIBSIZE;
			_IMG_FDUMP_LE(u32, bmpsize, file);
			// Reserved space (empty in this case, but can be anything) - 2 + 2 bytes
			_IMG_FDUMP_LE(u32, 0, file);
			// Starting adress of the pixel array - 4 bytes
			_IMG_FDUMP_LE(u32, HDRSIZE + DIBSIZE, file);
		}
		// FIXME: bmp uses int32_t (Windows' signed intiger) to store height/width but
		// representation of said int may not be portable as it should always be little endian (Intel) byte order
		// this is also a concern with other intigers used down here. Would be nice to have some kind of static check for those.
		// https://stackoverflow.com/questions/37368273/how-to-set-an-negative-value-to-the-resolution-of-a-bmp-image-in-hex
		{  // DIB header BITMAPINFOHEADER
			// the size of this header, in bytes (40) - 4 bytes
			_IMG_FDUMP_LE(u32, DIBSIZE, file);
			// the bitmap width in pixels (signed integer) - 4 bytes
			dbg_assert(width < INT32_MAX);
			_IMG_FDUMP_LE(i32, (i32)width, file);
			// the bitmap height in pixels (signed integer) - 4 bytes
			dbg_assert(height < INT32_MAX);
			_IMG_FDUMP_LE(i32, (i32)height, file);
			// the number of color planes (must be 1) - 2 bytes
			_IMG_FDUMP_LE(u16, 1, file);
			// the number of bits per pixel, which is the color depth of the image - 2 bytes
			_IMG_FDUMP_LE(u16, 32, file);
			// the compression method being used (0 for BI_RGB which stands for no compression) - 4 bytes
			_IMG_FDUMP_LE(u32, 0, file);
			// the image size. This is the size of the raw bitmap data; a dummy 0 can be given for BI_RGB bitmaps. - 4 bytes
			_IMG_FDUMP_LE(u32, 0, file);
			// FIXME: I'm not sure what the next two do but checked in stb_image and it sets them to 0
			// the horizontal resolution of the image. (pixel per metre, signed integer) - 4 bytes
			_IMG_FDUMP_LE(i32, 0, file);
			// the vertical resolution of the image. (pixel per metre, signed integer) - 4 bytes
			_IMG_FDUMP_LE(i32, 0, file);
			// the number of colors in the color palette, or 0 to default to 2n - 4 bytes
			_IMG_FDUMP_LE(u32, 0, file);
			// the number of important colors used, or 0 when every color is important; generally ignored - 4 bytes
			_IMG_FDUMP_LE(u32, 0, file);
		}
		break;
	} case IMG_TYPE_PNG: {
		{  // file header
			// Has the high bit set to detect transmission systems that do not support 8-bit data
			// and to reduce the chance that a text file is mistakenly interpreted as a PNG, or vice versa.
			_IMG_FDUMP_BE(u8, 0x89, file);
			// In ASCII, the letters PNG, allowing a person to identify the format easily if it is viewed in a text editor.
			_IMG_FDUMP_BE(u8, 'P', file);
			_IMG_FDUMP_BE(u8, 'N', file);
			_IMG_FDUMP_BE(u8, 'G', file);
			// A DOS-style line ending (CRLF) to detect DOS-Unix line ending conversion of the data.
			_IMG_FDUMP_BE(u8, 0x0D, file);
			_IMG_FDUMP_BE(u8, 0x0A, file);
			// A byte that stops display of the file under DOS when the command type has been used—the end-of-file character.
			_IMG_FDUMP_BE(u8, 0x1A, file);
			// A Unix-style line ending (LF) to detect Unix-DOS line ending conversion.
			_IMG_FDUMP_BE(u8, 0x0A, file);
		}
		{ // IHDR chunk
			// Length (4 bytes, 13)
			_IMG_FDUMP_BE(u32, 13, file);
			// Chunk type (4 bytes, IHDR in ASCII)
			_IMG_FDUMP_BE(u8, 'I', file);
			_IMG_FDUMP_BE(u8, 'H', file);
			_IMG_FDUMP_BE(u8, 'D', file);
			_IMG_FDUMP_BE(u8, 'R', file);
			// Chunk data (Lenght bytes)
			{  // IHDR chunk data
				// width (4 bytes)
				_IMG_FDUMP_BE(u32, width, file);
				// height (4 bytes)
				_IMG_FDUMP_BE(u32, height, file);
				// bit depth (1 byte, 8 for 8-bits per each channel)"
				_IMG_FDUMP_BE(u8, 8, file);
				// color type (1 byte, 6 for "Truecolor and alpha")
				_IMG_FDUMP_BE(u8, 6, file);
				// compression method (1 byte, 0 for no compression)
				_IMG_FDUMP_BE(u8, 0, file);
				// filter method (1 byte, 0 for no filtering)
				_IMG_FDUMP_BE(u8, 0, file);
				// interlace method (1 byte, 0 for "no interlace")
				_IMG_FDUMP_BE(u8, 0, file);
			}
			// CRC (4 bytes)
			// FIXME: CRC-32 calculation (for now set to 0 as placeholder)
			// https://en.wikipedia.org/wiki/Computation_of_cyclic_redundancy_checks
			dbg_warn("FIXME: CRC-32 calculation for .PNG");
			_IMG_FDUMP_BE(u32, 0, file);
		}
		{ // IDAT chunk (only beginning)
			// Length (4 bytes, 12)
			dbg_assert(width <= UINT32_MAX / height);
			const u32 length = width * height;
			_IMG_FDUMP_BE(u32, length, file);
			// Chunk type (4 bytes, IDAT in ASCII)
			_IMG_FDUMP_BE(u8, 'I', file);
			_IMG_FDUMP_BE(u8, 'D', file);
			_IMG_FDUMP_BE(u8, 'A', file);
			_IMG_FDUMP_BE(u8, 'T', file);
			// NOTE: the rest of IDAT chunk is filled in img_write and img_deinit
		}
		// NOTE: IEND chunk is filled in img_deinit
		break;
	} case IMG_TYPE_INVALID: {
	} default: {
		dbg_unreachable();
	} // end default
	} // end switch
	return true;
}

b8
img_deinit(struct img_context *ctx)
{
	dbg_assert(ctx != NULL);
	dbg_assert(ctx->_height * ctx->_width == ctx->_pixels);
	if (ctx->_type == IMG_TYPE_PNG) {
		{ // IDAT chunk (only the very end)
			// CRC (4 bytes)
			// FIXME: CRC-32 calculation (for now set to 0 as placeholder)
			// https://en.wikipedia.org/wiki/Computation_of_cyclic_redundancy_checks
			dbg_warn("FIXME: CRC-32 calculation for .PNG");
			_IMG_FDUMP_BE(u32, 0, ctx->_file);
		}
		{ // IEND chunk
			// Length (4 bytes, 0 for empty)
			_IMG_FDUMP_BE(u32, 0, ctx->_file);
			// Chunk type (4 bytes, IEND in ASCII)
			_IMG_FDUMP_BE(u8, 'I', ctx->_file);
			_IMG_FDUMP_BE(u8, 'E', ctx->_file);
			_IMG_FDUMP_BE(u8, 'N', ctx->_file);
			_IMG_FDUMP_BE(u8, 'D', ctx->_file);
			// CRC (4 bytes)
			// FIXME: CRC-32 calculation (for now set to 0 as placeholder)
			// https://en.wikipedia.org/wiki/Computation_of_cyclic_redundancy_checks
			dbg_warn("FIXME: CRC-32 calculation for .PNG");
			_IMG_FDUMP_BE(u32, 0, ctx->_file);
		}
	}
	const int err = fclose(ctx->_file);
	if (err != 0) return false;
	dbg_log("File closed.");
	return true;
}

void
img_write(struct img_context *ctx, struct img_color col)
{
	dbg_assert(ctx != NULL);
	dbg_assert(ctx->_height * ctx->_width > ctx->_pixels);
	switch (ctx->_type) {
	case IMG_TYPE_PPM:
		_IMG_FPRINTF(
			ctx->_file,
			"%"PRIu8" %"PRIu8" %"PRIu8"\n",
			col.r,
			col.g,
			col.b);
		(void)col.a;
		break;
	case IMG_TYPE_BMP:
		_IMG_FDUMP_LE(u8, col.b, ctx->_file);
		_IMG_FDUMP_LE(u8, col.g, ctx->_file);
		_IMG_FDUMP_LE(u8, col.r, ctx->_file);
		_IMG_FDUMP_LE(u8, col.a, ctx->_file);
		break;
	case IMG_TYPE_PNG:
		// NOTE: this order of channels already applies proper Endian
		_IMG_FDUMP_BE(u8, col.b, ctx->_file);
		_IMG_FDUMP_BE(u8, col.g, ctx->_file);
		_IMG_FDUMP_BE(u8, col.r, ctx->_file);
		_IMG_FDUMP_BE(u8, col.a, ctx->_file);
		// FIXME: CRC-32 calculation
		break;
	case IMG_TYPE_INVALID:
	default:
		dbg_unreachable();
	}
	#ifndef DBG_DISABLED
		ctx->_pixels += 1;
	#endif
}

u64
img_val_from_coords(struct img_context *ctx, u32 x, u32 y)
{
	dbg_assert(ctx != NULL);
	dbg_assert(x < ctx->_width);
	dbg_assert(y < ctx->_height);

	// NOTE: BMP stores images bottom-to-top by default.
	// said format supports flipping the image by setting the height
	// to negative value but after testing this out it seems image viewers
	// do not handle it very well - e.g. some may flip the image as expected
	// but still display negative height. I don't wanna relay on this and
	// prefer to flip the image manually.
	if (ctx->_type == IMG_TYPE_BMP)
		y = ctx->_height - y - 1;

	const i64 adj_x = (i64)x - (i64)(ctx->_start_x);
	const i64 adj_y = (i64)(ctx->_start_y) - (i64)y;
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
	dbg_assert((u64)val <= UINT64_MAX - ctx->_start_val + 1);
	const u64 out = (u64)val + ctx->_start_val - 1;
	return out;
}

u64
img_val_max(struct img_context *ctx)
{
	dbg_assert(ctx != NULL);
	dbg_assert(ctx->_width <= UINT64_MAX / ctx->_height);
	dbg_assert(ctx->_height <= UINT64_MAX / ctx->_width);
	// FIXME: the math for adj_width/height is way too ugly...
	const u32 adj_width = (ctx->_start_x < ctx->_width / 2) ? ((ctx->_width - ctx->_start_x) * 2 + 1) : (ctx->_start_x * 2 + 1);
	const u32 adj_height = (ctx->_start_y < ctx->_height / 2) ? ((ctx->_height - ctx->_start_y) * 2 + 1) : (ctx->_start_y * 2 + 1);
	const u32 max = _img_max(adj_width, adj_height);
	const u64 out = _img_pow2(max) + ctx->_start_val;
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

