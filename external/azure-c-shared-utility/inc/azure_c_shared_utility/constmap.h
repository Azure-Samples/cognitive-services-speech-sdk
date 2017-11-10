// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

/** @file       constmap.h
*	@brief		ConstMap is a module that implements a read-only dictionary
*           of @c const char* keys to @c const char* values.
*/

#ifndef CONSTMAP_H
#define CONSTMAP_H

#ifdef __cplusplus
#include <cstddef>
extern "C"
{
#else
#include <stddef.h>
#endif


#include "azure_c_shared_utility/macro_utils.h"
#include "azure_c_shared_utility/crt_abstractions.h"
#include "azure_c_shared_utility/map.h"
#include "azure_c_shared_utility/umock_c_prod.h"

#define CONSTMAP_RESULT_VALUES \
    CONSTMAP_OK, \
    CONSTMAP_ERROR, \
    CONSTMAP_INVALIDARG, \
    CONSTMAP_KEYNOTFOUND

/** @brief Enumeration specifying the status of calls to various APIs in this  
 *  module.
 */ 
DEFINE_ENUM(CONSTMAP_RESULT, CONSTMAP_RESULT_VALUES);
 
typedef struct CONSTMAP_HANDLE_DATA_TAG* CONSTMAP_HANDLE;
 

/**
 * @brief   Creates a new read-only map from a map handle.
 *
 * @param   sourceMap   The map from which we will populate key,value
 *                      into the read-only map.
 *
 * @return  A valid @c CONSTMAP_HANDLE or @c NULL in case an error occurs.
 */
MOCKABLE_FUNCTION(, CONSTMAP_HANDLE, ConstMap_Create, MAP_HANDLE, sourceMap);

 /** 
  * @brief  Destroy a read-only map.  Deallocate memory associated with handle.
  * @param  handle      Handle to a read-only map.
  */
MOCKABLE_FUNCTION(, void, ConstMap_Destroy, CONSTMAP_HANDLE, handle);

 /** 
  * @brief  Clone a read-only map from another read-only map. 
  * @param  handle      Handle to a read-only map.
  * @return A valid @c CONSTMAP_HANDLE or @c NULL in case an error occurs.
  */
MOCKABLE_FUNCTION(, CONSTMAP_HANDLE, ConstMap_Clone, CONSTMAP_HANDLE, handle);

 /** 
  * @brief  Create a map handle populated from the read-only map.
  * @param  handle      Handle to a read-only map.
  * @return A valid @c MAP_HANDLE or @c NULL in case an error occurs.
  *  
  * The new MAP_HANDLE needs to be destroyed when it is no longer needed.
  */
MOCKABLE_FUNCTION(, MAP_HANDLE, ConstMap_CloneWriteable, CONSTMAP_HANDLE, handle);

/**
 * @brief   This function returns a true if the map contains a key 
 *			with the same value the parameter @p key.
 *
 * @param   handle      The handle to an existing map.
 * @param   key         The key that the caller wants checked.
 *
 * @return				The function returns @c true if the key exists 
 *						in the map and @c false if key is not found or 
 *						parameters are invalid.
 */
MOCKABLE_FUNCTION(, bool, ConstMap_ContainsKey, CONSTMAP_HANDLE, handle, const char*, key);

/**
 * @brief   This function returns @c true if at least one <key,value> pair 
 *			exists in the map where the entry's value is equal to the 
 *			parameter @c value.
 *
 * @param   handle          The handle to an existing map.
 * @param   value           The value that the caller wants checked.
 *
 * @return					The function returns @c true if the value exists 
 *							in the map and @c false if value is not found or 
 *							parameters are invalid.
 */
MOCKABLE_FUNCTION(, bool, ConstMap_ContainsValue, CONSTMAP_HANDLE, handle, const char*, value);

/**
 * @brief   Retrieves the value of a stored key.
 *
 * @param   handle  The handle to an existing map.
 * @param   key     The key to be looked up in the map.
 *
 * @return  Returns @c NULL in case the input arguments are @c NULL or if the
 *          requested key is not found in the map. Returns a pointer to the
 *          key's value otherwise.
 */
MOCKABLE_FUNCTION(, const char*, ConstMap_GetValue, CONSTMAP_HANDLE, handle, const char*, key);
 
 /**
 * @brief   Retrieves the complete list of keys and values from the map
 *          in @p values and @p keys. Also writes the size of the list
 *          in @p count.
 *
 * @param   handle      The handle to an existing map.
 * @param   keys        The location where the list of keys is to be written.
 * @param   values      The location where the list of values is to be written.
 * @param   count       The number of stored keys and values is written at the
 *                      location indicated by this pointer.
 *
 * @return  Returns @c CONSTMAP_OK if the keys and values are retrieved
 *                     and written successfully or an error code otherwise.
 */
MOCKABLE_FUNCTION(, CONSTMAP_RESULT, ConstMap_GetInternals, CONSTMAP_HANDLE, handle, const char*const**, keys, const char*const**, values, size_t*, count);


#ifdef __cplusplus
}
#endif

#endif /* CONSTMAP_H */
