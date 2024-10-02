
#ifndef IMG_H
#define IMG_H

#include <stdint.h>
#include <stdio.h>

enum ImgType {
	IMG_TYPE_INVALID,
	IMG_TYPE_PPM,
	// FIXME: BMP image is flipped
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
