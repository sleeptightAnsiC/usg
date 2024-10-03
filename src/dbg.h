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

// #define DBG_DISABLED

#ifndef DBG_DISABLED
# define DBG_CODE if (0); else
#else
# define DBG_CODE if (1); else
#endif

#ifndef DBG_DISABLED
# define DBG_STATIC_ASSERT(COND) _DBG_STATIC_ASSERT_INTERNAL((COND))
# define _DBG_CONCAT(prefix, suffix) prefix##suffix
# define _DBG_UNIQUE_NAME(prefix) _DBG_CONCAT(prefix##_, __LINE__)
# define _DBG_STATIC_ASSERT_INTERNAL(EXPRESSION) \
	struct _DBG_UNIQUE_NAME(STATIC_ASSERTION_FAILED) \
	{ char STATIC_ASSERTION_FAILED[(EXPRESSION) ? 1 : -1]; }
#else
# define DBG_STATIC_ASSERT(COND)
#endif

#ifndef DBG_DISABLED
# define _dbg_log(PREFIX, ...) \
	do { \
		/* fprintf(stderr, "[%s:%d:%s] %s: ", __FILE__, __LINE__, __func__, PREFIX); */ \
		fprintf(stderr, "[%s] %s: ", __func__, PREFIX); \
		fprintf(stderr, __VA_ARGS__); \
		fprintf(stderr, "\n"); \
	} while (0)
#else
# define _dbg_log(PREFIX, ...) \
	(void)(COND)
#endif


#ifndef DBG_DISABLED
# define dbg_log(...) \
	_dbg_log("log", __VA_ARGS__)
#else
# define dbg_log(...) \
	(void)(__VA_ARGS__)
#endif


#ifndef DBG_DISABLED
# define dbg_error(...) \
	_dbg_log("ERROR", __VA_ARGS__)
#else
# define dbg_error(...) \
	(void)(__VA_ARGS__)
#endif


#ifndef DBG_DISABLED
# define dbg_assert(COND) \
	do{ \
		if (errno != 0) \
			dbg_error("%s (errno)", strerror(errno)); \
		assert(COND); \
	} while (0)
#else
// FIXME: COND might be a function call so this should be just discarded
# define dbg_assert(COND) \
	(void)(COND)
#endif


// TODO: compiler specific function can be added here
#ifndef DBG_DISABLED
# define dbg_unreachable() \
	do{ \
		dbg_error("This code should never be reached!"); \
		assert(0); \
	} while (0)
#else
# define dbg_unreachable()
#endif


#endif //DBG_H


