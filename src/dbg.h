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

#define _dbg_log(PREFIX, ...) \
	(void)( \
		fprintf(stderr, "%s[%s:%d:%s()] ", PREFIX, __FILE__, __LINE__, __func__), \
		fprintf(stderr, __VA_ARGS__), \
		fprintf(stderr, "\n"), \
	0)

#define dbg_log(...) \
	_dbg_log("[info]", __VA_ARGS__)

#define dbg_error(...) \
	_dbg_log("[ERR!]", __VA_ARGS__)

#define dbg_warn(...) \
	_dbg_log("[warn]", __VA_ARGS__)

#define dbg_assert(COND) \
	(void)( \
		(!(COND) && errno != 0) ? \
			dbg_error("%s (errno)", strerror(errno)) \
		: \
			(void)0, \
		assert(COND), \
	0)

#define dbg_unreachable() \
	(void)( \
		dbg_error("This code should never be reached!"), \
		assert(0), \
	0)

#else  // DBG_DISABLED

#define dbg_log(...) ((void)0)
#define dbg_error(...) ((void)0)
#define dbg_warn(...) ((void)0)
#define dbg_assert(COND) ((void)0)

#if defined(__GNUC__)
	#define dbg_unreachable() __builtin_unreachable()
#elif defined(_MSC_VER)
	#define dbg_unreachable() __assume(0)
#else
	#define dbg_unreachable() ((void)0)
#endif

#endif  // DBG_DISABLED

#endif //DBG_H


