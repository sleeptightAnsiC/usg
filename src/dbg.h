#ifndef DBG_H
#define DBG_H

#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

/* DBG
 * Debugging utilities
 *
 * DBG_DISABLED
 *	Defining this macro strips any debug functionality
 */

// NOTE: static assert is always there,
// because it changes nothing in shipping build
// and because it may appear outside of function body
#define _DBG_STATIC_ASSERT_3(COND,MSG) struct STATIC_ASSERTION_##MSG { char STATIC_ASSERTION_##MSG[(COND) ? 1 : -1]; }
#define _DBG_STATIC_ASSERT_2(COND,L) _DBG_STATIC_ASSERT_3(COND,AT_LINE_##L)
#define _DBG_STATIC_ASSERT_1(COND,L) _DBG_STATIC_ASSERT_2(COND,L)
#define DBG_STATIC_ASSERT(COND) _DBG_STATIC_ASSERT_1(COND,__LINE__)

// TODO: I think it would be better if debug macros
// were disabled by default, so this should be DBG_ENABLED
#ifndef DBG_DISABLED

// FIXME: this macro is problematic because
// it often uses dbg_log/assert/etc inside of the block
// and produces warning about unused variables
// when compiling with DBG_DISABLED
#define DBG_CODE \
	if (0); else

#define _dbg_log(PREFIX, ...) \
	(void)( \
		fprintf(stderr, "[%s] %s: ", __func__, PREFIX), \
		fprintf(stderr, __VA_ARGS__), \
		fprintf(stderr, "\n"), \
	0)

#define dbg_log(...) \
	_dbg_log("log", __VA_ARGS__)

#define dbg_error(...) \
	_dbg_log("ERROR", __VA_ARGS__)

#define dbg_assert(COND) \
	(void)( \
		(!(COND) && errno != 0) ? \
			dbg_error("%s (errno)", strerror(errno)) \
		: \
			(void)0, \
		assert(COND), \
	0)

// TODO: compiler specific function can be added here
#define dbg_unreachable() \
	(void)( \
		dbg_error("This code should never be reached!"), \
		assert(0), \
	0)

#else  // DBG_DISABLED

#define DBG_CODE if (1); else
#define dbg_log(...) ((void)0)
#define dbg_error(...) ((void)0)
#define dbg_assert(COND) ((void)0)

#if defined(__GNUC__)
#	define dbg_unreachable() __builtin_unreachable()
#elif defined(_MSC_VER)
#	define dbg_unreachable() __assume(0)
#else
#	define dbg_unreachable() ((void)0)
#endif

#endif  // DBG_DISABLED

#endif //DBG_H


