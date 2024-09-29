#include <stdint.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "./dbg.h"
#include "./soe.h"

int
main(void)
{
	// const uint64_t max = 1080 * 1080;
	// const uint64_t max = 1920 * 1080 * 1000;
	// const uint64_t max = (UINT64_MAX - 1) / 1000000000;
	const uint64_t max = (UINT64_MAX - 1);
	const struct SoeCache cache = soe_init(max);
	(void)soe_is_prime(cache, max);
	dbg_assert(soe_is_prime(cache, 2111) == true);
	dbg_assert(soe_is_prime(cache, 2113) == true);
	dbg_assert(soe_is_prime(cache, 2114) == false);
	dbg_assert(soe_is_prime(cache, 2115) == false);
	soe_deinit(cache);
	return EXIT_SUCCESS;
}

