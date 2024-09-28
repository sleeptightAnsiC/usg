#include <stdint.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "./soe.h"

int
main(void)
{
	const struct SoeCache cache = soe_populate(1920 * 1080);
	// const struct SoeCache cache = soe_populate(15);
	(void)soe_is_prime(cache, 2);
	(void)soe_is_prime(cache, 11);
	(void)soe_is_prime(cache, 15);
	(void)soe_is_prime(cache, 2113);
	(void)soe_is_prime(cache, 2114);
	(void)soe_is_prime(cache, 1111001);
	(void)soe_is_prime(cache, 1920 * 1080);
	return EXIT_SUCCESS;
}

