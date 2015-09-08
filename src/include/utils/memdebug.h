/*-------------------------------------------------------------------------
 *
 * memdebug.h
 *	  Memory debugging support.
 *
 * Currently, this file either wraps <valgrind/memcheck.h> or substitutes
 * empty definitions for Valgrind client request macros we use.
 *
 *
 * Portions Copyright (c) 1996-2015, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/utils/memdebug.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef MEMDEBUG_H
#define MEMDEBUG_H

#ifdef USE_VALGRIND
#include <valgrind/memcheck.h>

#elif __has_feature(address_sanitizer) || defined(__SANITIZE_ADDRESS__)

#include <sanitizer/asan_interface.h>

#define VALGRIND_MAKE_MEM_DEFINED(addr, size) \
	ASAN_UNPOISON_MEMORY_REGION(addr, size)

#define VALGRIND_MAKE_MEM_UNDEFINED(addr, size) \
	ASAN_UNPOISON_MEMORY_REGION(addr, size)

#define VALGRIND_MEMPOOL_ALLOC(context, ret, size) \
	ASAN_UNPOISON_MEMORY_REGION(ret, size)

#define VALGRIND_MAKE_MEM_NOACCESS(addr, size) \
	ASAN_POISON_MEMORY_REGION(addr, size)

#define VALGRIND_CHECK_MEM_IS_DEFINED(addr, size)			do {} while (0)
#define VALGRIND_CREATE_MEMPOOL(context, redzones, zeroed)	do {} while (0)
#define VALGRIND_DESTROY_MEMPOOL(context)					do {} while (0)
#define VALGRIND_MEMPOOL_FREE(context, addr)				do {} while (0)
#define VALGRIND_MEMPOOL_CHANGE(context, optr, nptr, size)	do {} while (0)

#else
#define VALGRIND_CHECK_MEM_IS_DEFINED(addr, size)			do {} while (0)
#define VALGRIND_CREATE_MEMPOOL(context, redzones, zeroed)	do {} while (0)
#define VALGRIND_DESTROY_MEMPOOL(context)					do {} while (0)
#define VALGRIND_MAKE_MEM_DEFINED(addr, size)				do {} while (0)
#define VALGRIND_MAKE_MEM_NOACCESS(addr, size)				do {} while (0)
#define VALGRIND_MAKE_MEM_UNDEFINED(addr, size)				do {} while (0)
#define VALGRIND_MEMPOOL_ALLOC(context, addr, size)			do {} while (0)
#define VALGRIND_MEMPOOL_FREE(context, addr)				do {} while (0)
#define VALGRIND_MEMPOOL_CHANGE(context, optr, nptr, size)	do {} while (0)
#endif

#endif   /* MEMDEBUG_H */
