
#ifndef ULS_H
#define ULS_H

#include <stdint.h>

struct UlsContext {
	uint32_t _start;
	uint32_t _radius;
};

struct UlsContext uls_init(uint32_t start, uint32_t radius);
uint64_t uls_value_at_coordinate(struct UlsContext ctx, uint32_t x, uint32_t y);

#endif

