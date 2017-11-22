// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

/** @file threadapi.h
 *	@brief	 This module implements support for creating new threads,
 *			 terminating threads and sleeping threads.
 */

#ifndef THREADAPI_H
#define THREADAPI_H

#ifdef __cplusplus
#include <cstdint>
extern "C" {
#else
#include <stdint.h>
#endif

#include "azure_c_shared_utility/macro_utils.h"
#include "azure_c_shared_utility/umock_c_prod.h"
    
typedef int(*THREAD_START_FUNC)(void *);

#define THREADAPI_RESULT_VALUES \
    THREADAPI_OK,               \
    THREADAPI_INVALID_ARG,      \
    THREADAPI_NO_MEMORY,        \
    THREADAPI_ERROR

/** @brief Enumeration specifying the possible return values for the APIs in
 *		   this module.
 */
DEFINE_ENUM(THREADAPI_RESULT, THREADAPI_RESULT_VALUES);

typedef void* THREAD_HANDLE;

#ifdef __linux__
typedef long THREADAPI_ID;
#else
typedef uint32_t THREADAPI_ID;
#endif

/**
 * @brief	Creates a thread with the entry point specified by the @p func
 * 			argument.
 *
 * @param   threadHandle	The handle to the new thread is returned in this
 * 							pointer.
 * @param	func			A function pointer that indicates the entry point
 * 							to the new thread.
 * @param   arg				A void pointer that must be passed to the function
 * 							pointed to by @p func.
 *
 * @return	@c THREADAPI_OK if the API call is successful or an error
 * 			code in case it fails.
 */
MOCKABLE_FUNCTION(, THREADAPI_RESULT, ThreadAPI_Create, THREAD_HANDLE*, threadHandle, THREAD_START_FUNC, func, void*, arg);

/**
 * @brief	Blocks the calling thread by waiting on the thread identified by
 * 			the @p threadHandle argument to complete.
 *
 * @param	threadHandle	The handle of the thread to wait for completion.
 * @param   res 			The result returned by the thread which is passed
 * 							to the ::ThreadAPI_Exit function.
 *
 *			When the @p threadHandle thread completes, all resources associated
 *			with the thread must be released and the thread handle will no
 *			longer be valid.
 * 
 * @return	@c THREADAPI_OK if the API call is successful or an error
 * 			code in case it fails.
 */
MOCKABLE_FUNCTION(, THREADAPI_RESULT, ThreadAPI_Join, THREAD_HANDLE, threadHandle, int*, res);

/**
 * @brief	This function is called by a thread when the thread exits.
 *
 * @param	res		An integer that represents the exit status of the thread.
 * 				
 * 			This function is called by a thread when the thread exits in order
 * 			to return a result value to the caller of the ::ThreadAPI_Join
 * 			function. The @p res value must be copied into the @p res out
 * 			argument passed to the ::ThreadAPI_Join function.
 */
MOCKABLE_FUNCTION(, void, ThreadAPI_Exit, int, res);

/**
 * @brief	Sleeps the current thread for the given number of milliseconds.
 *
 * @param	milliseconds	The number of milliseconds to sleep.
 */
MOCKABLE_FUNCTION(, void, ThreadAPI_Sleep, unsigned int, milliseconds);

/**
 * @brief	Returns an ID for the current thread that is recognized by the host
 *			operating system.
 *
 * @return	@c The thread ID.
 */
MOCKABLE_FUNCTION(, THREADAPI_ID, ThreadAPI_GetCurrentId);

#ifdef __cplusplus
}
#endif

#endif /* THREADAPI_H */
