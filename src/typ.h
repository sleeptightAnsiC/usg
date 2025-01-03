
#ifndef _TYP_H
#define _TYP_H


// WARN:
// I always expect that:
//	sizeof(byte) == 1
//	sizeof(_Bool) == 1
//	sizeof(float) == 4
//	sizeof(double) == 8
// and that 'long double' is at least 80bit float
// otherwise, I would trully lose my sanity...


#if defined(_WIN32)
	#include <basetsd.h>
	typedef UINT64 u64;
	typedef UINT32 u32;
	typedef UINT16 u16;
	typedef UINT8 u8;
	typedef INT64 i64;
	typedef INT32 i32;
	typedef INT16 i16;
	typedef INT8 i8;
#elif defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
	#include <stdint.h>
	typedef uint64_t u64;
	typedef uint32_t u32;
	typedef uint16_t u16;
	typedef uint8_t u8;
	typedef int64_t i64;
	typedef int32_t i32;
	typedef int16_t i16;
	typedef int8_t i8;
#else
	#error "Unable to define intiger types!"
#endif


#if defined(__cplusplus)
	typedef bool b8;
#elif defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
	typedef _Bool b8;
#elif defined(_WIN32)
	typedef i8 b8;
#else
	#error "Unable to define boolean type!"
#endif

#if !defined(__cplusplus) && !defined(true)
	#define true (1)
#endif

#if !defined(__cplusplus) && !defined(false)
	#define false (0)
#endif


typedef float f32;
typedef double f64;
typedef long double f80;


#endif  // _TYP_H

