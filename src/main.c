#include <stdint.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "./dbg.h"
#include "./soe.h"
#include "./spr.h"

int
main(void)
{
	// FIXME: small max value is causing a crash for some reason
	// const uint64_t max = 1080 * 1080;
	// const uint64_t max = 1920 * 1080 * 1000;
	// const uint64_t max = (UINT64_MAX - 1) / 1000000000;
	// const uint64_t max = (UINT64_MAX - 1);
	const uint64_t wh = 21;
	const uint64_t max = wh * wh;
	const struct SoeCache cache = soe_init(max);
	// (void)soe_is_prime(cache, max);
	// dbg_assert(soe_is_prime(cache, 2111) == true);
	for (uint64_t i = 0; i < wh; ++i) {
		for (uint64_t j = 0; j < wh; ++j) {
			const int64_t x = spr_screen_to_coord_x(j, wh);
			const int64_t y = spr_screen_to_coord_y(i, wh);
			const uint64_t val = spr_coords_to_val(x,y);
			const bool prime = soe_is_prime(cache, val);
			const char sign = prime ? 'X' : '.';
			putchar(' ');
			putchar(sign);
		}
		putchar('\n');
	}
	soe_deinit(cache);
	return EXIT_SUCCESS;
}

