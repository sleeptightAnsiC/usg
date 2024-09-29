#include <stdint.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "./soe.h"

int
main(void)
{
	const uint64_t max = 1080 * 1080;
	// const uint64_t max = 1920 * 1080 * 1000;
	// const uint64_t max = (UINT64_MAX - 1) / 1000000000;
	const struct SoeCache cache = soe_init(max);
	for (uint64_t i = 1; i <= 40; ++i)
		(void)soe_is_prime(cache, i);
	(void)soe_is_prime(cache, 2113);
	(void)soe_is_prime(cache, 2114);
	(void)soe_is_prime(cache, 1111001);
	(void)soe_is_prime(cache, max);
	soe_deinit(cache);
	return EXIT_SUCCESS;
}

