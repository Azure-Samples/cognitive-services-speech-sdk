// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

/** @file lock.h  
*	@brief		A minimalistic platform agnostic lock abstraction for thread
*				synchronization.
*	@details	The Lock component is implemented in order to achieve thread
*				synchronization, as we may have a requirement to consume locks
*				across different platforms. This component exposes some generic
*				APIs so that it can be extended for platform specific
*				implementations.
*/

#ifndef LOCK_H
#define LOCK_H

#include "azure_c_shared_utility/macro_utils.h"
#include "azure_c_shared_utility/umock_c_prod.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void* LOCK_HANDLE;

#define LOCK_RESULT_VALUES \
    LOCK_OK, \
    LOCK_ERROR \

/** @brief Enumeration specifying the lock status.
*/
DEFINE_ENUM(LOCK_RESULT, LOCK_RESULT_VALUES);

/**
 * @brief	This API creates and returns a valid lock handle.
 *
 * @return	A valid @c LOCK_HANDLE when successful or @c NULL otherwise.
 */
MOCKABLE_FUNCTION(, LOCK_HANDLE, Lock_Init);

/**
 * @brief	Acquires a lock on the given lock handle. Uses platform
 * 			specific mutex primitives in its implementation.
 *
 * @param	handle	A valid handle to the lock.
 *
 * @return	Returns @c LOCK_OK when a lock has been acquired and
 * 			@c LOCK_ERROR when an error occurs.
 */
MOCKABLE_FUNCTION(, LOCK_RESULT, Lock, LOCK_HANDLE, handle);

/**
 * @brief	Releases the lock on the given lock handle. Uses platform
 * 			specific mutex primitives in its implementation.
 *
 * @param	handle	A valid handle to the lock.
 *
 * @return	Returns @c LOCK_OK when the lock has been released and
 * 			@c LOCK_ERROR when an error occurs.
 */
MOCKABLE_FUNCTION(, LOCK_RESULT, Unlock, LOCK_HANDLE, handle);

/**
 * @brief	The lock instance is destroyed.
 *
 * @param	handle	A valid handle to the lock.
 *
 * @return	Returns @c LOCK_OK when the lock object has been
 * 			destroyed and @c LOCK_ERROR when an error occurs.
 */
MOCKABLE_FUNCTION(, LOCK_RESULT, Lock_Deinit, LOCK_HANDLE, handle);

#ifdef __cplusplus
}
#endif

#endif /* LOCK_H */
