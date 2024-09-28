#ifndef DBG_H
#define DBG_H

#include <assert.h>


// TODO: disable assertions and debug-only macros in shipping builds

#define DBG_CODE if (false); else

// NOTE: this was taken from mtpl
// FIXME: rlspr uses a bit different static assert, but it causes errors in mtpl
#define DBG_STATIC_ASSERT(COND) _DBG_STATIC_ASSERT_INTERNAL((COND))
#define _DBG_CONCAT(prefix, suffix) prefix##suffix
#define _DBG_UNIQUE_NAME(prefix) _DBG_CONCAT(prefix##_, __LINE__)
#define _DBG_STATIC_ASSERT_INTERNAL(EXPRESSION) \
	struct _DBG_UNIQUE_NAME(STATIC_ASSERTION_FAILED) \
	{ char STATIC_ASSERTION_FAILED[(EXPRESSION) ? 1 : -1]; } \

#define dbg_assert(COND) \
	assert(COND) \

#define dbg_unreachable() \
	do{ \
		assert("This code should never be reached!" && 0); \
	} while (0) \


#endif //DBG_H


// leftovers from when this file was called "util.h"
#if 0
#define UTIL_MIN(A, B) (((A) < (B)) ? (A) : (B))
#define UTIL_MAX(A, B) (((A) > (B)) ? (A) : (B))
#define UTIL_ARRNUM(ARR) (sizeof(ARR)/sizeof(ARR[0]))
#endif
