
#include "./img.h"
#include "./dbg.h"
#include <inttypes.h>
#include <stdio.h>


#define _img_fprintf(...) \
	do { \
		const int _result = fprintf(__VA_ARGS__); \
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
	case IMG_TYPE_PPM:
		_img_fprintf(file, "P3\n");
		_img_fprintf(file, "%"PRIu32" %"PRIu32"\n", w, h);
		_img_fprintf(file, "255\n");
		DBG_STATIC_ASSERT(UINT8_MAX == 255);
		break;
	case IMG_TYPE_BMP:
	case IMG_TYPE_INVALID:
	default:
		dbg_unreachable();
	}
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
	DBG_CODE { ctx->_pixels += 1; }
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
	case IMG_TYPE_INVALID:
	default:
		dbg_unreachable();
	}
}

