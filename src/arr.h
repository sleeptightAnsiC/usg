
#ifndef ARR_H
#define ARR_H

#include <stdlib.h>  // malloc free realloc NULL size_t
#include "./dbg.h"


#define ARR_STRUCT(NAME, DATA_T, NUM_T) \
	struct NAME { \
		DATA_T *_data; \
		NUM_T _len; \
		NUM_T _cap; \
	} \


#define arr_init_null(ARR, NUM) \
	do { \
		dbg_assert((NUM) > 0); \
		(ARR)._len = (NUM); \
		(ARR)._cap = (NUM); \
		(ARR)._data = calloc((size_t)((ARR)._cap), sizeof((ARR)._data[0])); \
		dbg_assert((ARR)._data != NULL); \
	} while(0) \

#define arr_init_cap(ARR, CAP) \
	do { \
		dbg_assert((CAP) > 0); \
		(ARR)._len = 0; \
		(ARR)._cap = (CAP); \
		_arr_malloc(ARR); \
	} while(0) \

#define arr_deinit(ARR) \
	do { \
		free((ARR)._data); \
	} while(0) \

#define arr_push_back_uninitialized(ARR) \
	do { \
		dbg_assert((ARR)._cap >= (ARR)._len); \
		if ((ARR)._cap == (ARR)._len) { \
			(ARR)._cap *= 2; \
			_arr_realloc(ARR); \
		} \
		(ARR)._len += 1; \
	} while(0) \

#define arr_push_back(ARR, VAL) \
	do { \
		arr_push_back_uninitialized(ARR); \
		arr_back(ARR) = (VAL); \
	} while(0) \

#define arr_pop_back(ARR) \
	do { \
		(ARR)._len -= 1; \
	} while(0) \

#define arr_pop_all(ARR) \
	do { \
		(ARR)._len = 0; \
	} while(0) \

#define arr_shrink_to_fit(ARR) \
	do { \
		(ARR)._cap = (ARR)._len; \
		_arr_realloc(ARR); \
	} while(0) \

#define arr_front(ARR) \
	((ARR)._data[0]) \

#define arr_back(ARR) \
	((ARR)._data[(ARR)._len - 1]) \

#define arr_at(ARR, NUM) \
	((ARR)._data[(NUM)]) \

#define arr_size(ARR) \
	((ARR)._len) \


#define arr_data_size(ARR) \
	(sizeof((ARR)._data[0]) * (size_t)((ARR)._cap)) \

#define _arr_realloc(ARR) \
	do { \
		dbg_assert((ARR)._cap > 0); \
		const size_t _size = arr_data_size(ARR); \
		(ARR)._data = realloc((ARR)._data, _size); \
		dbg_assert((ARR)._data); \
	} while(0) \

#define _arr_malloc(ARR) \
	do { \
		dbg_assert((ARR)._cap > 0); \
		const size_t _size = arr_data_size(ARR); \
		(ARR)._data = malloc(_size); \
		dbg_assert((ARR)._data); \
	} while(0) \

#endif // ARR_H

