#ifndef _CCL_H
#define _CCL_H

#include "./typ.h"
#include "./dbg.h"

/* CCL
 * Compiler Compatibility Layer
 *
 * Macros for wraping compiler-specific constructs
 * or for determining platform-specific behavior.
 */


// Bit order detection (Endianness)
// https://en.wikipedia.org/wiki/Endianness
// https://stackoverflow.com/questions/2100331/macro-definition-to-determine-big-endian-or-little-endian-machine/2103095#2103095
// https://stackoverflow.com/questions/29113768/visual-studio-determine-endianness
//
// This will print "Hi!" only on Little Endian machines:
//	if (CCL_ENDIAN_ORDER == CCL_ENDIAN_LITTLE) printf("Hi!");

// same values as the ones defined by gcc-compatible compilers such as:
// cc, clang, tcc (mob-devel), icx and other clang-derivatives
#define CCL_ENDIAN_LITTLE (1234)
#define CCL_ENDIAN_BIG (4321)
#define CCL_ENDIAN_PDP (3412)

#if defined(__BYTE_ORDER__)
	// if __BYTE_ORDER__ is defined, we can safely use it
	// and we can asume that __ORDER_LITTLE_ENDIAN__
	// __ORDER_BIG_ENDIAN__ and __ORDER_PDP_ENDIAN__ are also defined
	#define CCL_ENDIAN_ORDER __BYTE_ORDER__
	// TCC mob-devel seems to be missing this one single define
	#if defined(__TINYC__) && !defined(__ORDER_PDP_ENDIAN__)
		#define __ORDER_PDP_ENDIAN__ CCL_ENDIAN_PDP
	#endif
	// check if our macros match with those predefined
	// this will also fail if some of them are missing
	#if (CCL_ENDIAN_LITTLE != __ORDER_LITTLE_ENDIAN__) || (CCL_ENDIAN_BIG != __ORDER_BIG_ENDIAN__) || (CCL_ENDIAN_PDP != __ORDER_PDP_ENDIAN__)
		#error "Mismatch between CCL_ENDIAN_* and __ORDER_*_ENDIAN__ macros detected !"
	#endif
#elif defined(_MSC_VER)
	// MSVC does NOT predefine anything about Endianess
	// and this compiler only suports targets with Little Endian
	// even MSVC's STL hardcodes it to always be Little Endian
	// https://github.com/microsoft/STL/blob/7643c270e5bfb1cfad62f8b5ff4045c662bdaf81/stl/inc/bit#L230
	#define CCL_ENDIAN_ORDER CCL_ENDIAN_LITTLE
#else
	#error "Unable to deremine CCL_ENDIAN_ORDER !"
#endif

// May trigger someday for something like Honeywell 316 (aka ENDIAN_BIG_WORD)
// currently nothing I tested really supports this Endianess
#if (CCL_ENDIAN_ORDER != CCL_ENDIAN_LITTLE) && (CCL_ENDIAN_ORDER != CCL_ENDIAN_BIG) && (CCL_ENDIAN_ORDER != CCL_ENDIAN_PDP)
	#error "Unknown CCL_ENDIAN_ORDER !"
#endif


// NOTE: this is a workaround for TCC as it does not support _Pragma
// https://lists.nongnu.org/archive/html/tinycc-devel/2024-12/msg00009.html
#if defined(__TINYC__)
	#define CCL_PRAGMA(...)
#else
	#define CCL_PRAGMA(...) _Pragma(__VA_ARGS__)
#endif


#endif  // _CCL_H
