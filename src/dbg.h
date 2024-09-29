#ifndef DBG_H
#define DBG_H

#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>


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

#define _dbg_log(PREFIX, ...) \
	do { \
		/* fprintf(stderr, "[%s:%d:%s] %s: ", __FILE__, __LINE__, __func__, PREFIX); */ \
		fprintf(stderr, "[%s] %s: ", __func__, PREFIX); \
		fprintf(stderr, __VA_ARGS__); \
		fprintf(stderr, "\n"); \
	} while (0) \

#define dbg_log(...) \
	_dbg_log("log", __VA_ARGS__) \

#define dbg_error(...) \
	_dbg_log("ERROR", __VA_ARGS__) \

#define dbg_assert(COND) \
	do{ \
		if (errno != 0) \
			dbg_error("%s (errno)", strerror(errno)); \
		assert(COND); \
	} while (0) \

#define dbg_unreachable() \
	do{ \
		dbg_error("This code should never be reached!"); \
		assert(0); \
	} while (0) \


#endif //DBG_H


// leftovers from when this file was called "util.h"
#if 0
#define UTIL_MIN(A, B) (((A) < (B)) ? (A) : (B))
#define UTIL_MAX(A, B) (((A) > (B)) ? (A) : (B))
#define UTIL_ARRNUM(ARR) (sizeof(ARR)/sizeof(ARR[0]))
#endif
