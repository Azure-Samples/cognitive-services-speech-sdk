// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

/** @file       map.h
*	@brief		Map is a module that implements a dictionary of @c STRING_HANDLE
*               keys to @c STRING_HANDLE values.
*/

#ifndef MAP_H
#define MAP_H

#ifdef __cplusplus
#include <cstddef>
extern "C"
{
#else
#include <stddef.h>
#endif


#include "azure_c_shared_utility/macro_utils.h"
#include "azure_c_shared_utility/strings.h"
#include "azure_c_shared_utility/crt_abstractions.h"
#include "azure_c_shared_utility/umock_c_prod.h"

#define MAP_RESULT_VALUES \
    MAP_OK, \
    MAP_ERROR, \
    MAP_INVALIDARG, \
    MAP_KEYEXISTS, \
    MAP_KEYNOTFOUND, \
    MAP_FILTER_REJECT

/** @brief Enumeration specifying the status of calls to various APIs in this  
 *  module.
 */
DEFINE_ENUM(MAP_RESULT, MAP_RESULT_VALUES);

typedef struct MAP_HANDLE_DATA_TAG* MAP_HANDLE;

typedef int (*MAP_FILTER_CALLBACK)(const char* mapProperty, const char* mapValue);

/**
 * @brief   Creates a new, empty map.
 *
 * @param   mapFilterFunc   A callback function supplied by the caller that is
 *                          invoked during calls to ::Map_Add and
 *                          ::Map_AddOrUpdate to provide the caller an
 *                          opportunity to indicate whether the new entry or
 *                          the update to an existing entry can be made or not.
 *                          The callback function can request that the operation
 *                          be canceled by returning a non-zero value from the
 *                          callback.
 *
 * @return  A valid @c MAP_HANDLE or @c NULL in case an error occurs.
 */
MOCKABLE_FUNCTION(, MAP_HANDLE, Map_Create, MAP_FILTER_CALLBACK, mapFilterFunc);

/**
 * @brief   Release all resources associated with the map.
 *
 * @param   handle  The handle to an existing map.
 */
MOCKABLE_FUNCTION(, void, Map_Destroy, MAP_HANDLE, handle);

/**
 * @brief   Creates a copy of the map indicated by @p handle and returns a
 *          handle to it.
 *
 * @param   handle  The handle to an existing map.
 *
 * @return  A valid @c MAP_HANDLE to the cloned copy of the map or @c NULL
 *          in case an error occurs.
 */
MOCKABLE_FUNCTION(, MAP_HANDLE, Map_Clone, MAP_HANDLE, handle);

/**
 * @brief   Adds a key/value pair to the map.
 *
 * @param   handle  The handle to an existing map.
 * @param   key     The @c key to be used for this map entry.
 * @param   value   The @c value to be associated with @p key.
 *
 *          If a non-NULL pointer to a callback function was supplied
 *          via the @c mapFilterFunc parameter when ::Map_Create was
 *          called then that callback is invoked when a new entry is
 *          added and if the callback returns a non-zero value then
 *          the function cancels the add operation and returns
 *          @c MAP_FILTER_REJECT.
 * 
 * @return  If any of the input parameters are @c NULL then this function
 *          returns @c MAP_INVALID_ARG. If the key already exists in the
 *          map then @c MAP_KEYEXISTS is returned. If the filter function
 *          associated with the map rejects the entry then
 *          @c MAP_FILTER_REJECT is returned. In case an error occurs when
 *          the new key is added to the map the function returns @c MAP_ERROR.
 *          If everything goes well then @c MAP_OK is returned.
 */
MOCKABLE_FUNCTION(, MAP_RESULT, Map_Add, MAP_HANDLE, handle, const char*, key, const char*, value);

/**
 * @brief   Adds/updates a key/value pair to the map.
 *
 * @param   handle  The handle to an existing map.
 * @param   key     The @c key to be used for this map entry.
 * @param   value   The @c value to be associated with @p key.
 *
 *          This function behaves exactly like ::Map_Add except that if the key
 *          already exists in the map then it overwrites the value with the
 *          supplied value instead of returning an error. If a non-NULL pointer
 *          to a callback function was supplied via the @c mapFilterFunc parameter
 *          when ::Map_Create was called then that callback is invoked when a new
 *          entry is added or when an existing entry is updated and if the
 *          callback returns a non-zero value then the function cancels the
 *          add/update operation and returns @c MAP_FILTER_REJECT.
 * 
 * @return  If any of the input parameters are @c NULL then this function
 *          returns @c MAP_INVALID_ARG. If the filter function associated
 *          with the map rejects the entry then @c MAP_FILTER_REJECT is
 *          returned. In case an error occurs when the new key is
 *          added/updated in the map the function returns @c MAP_ERROR. If
 *          everything goes well then @c MAP_OK is returned.
 */
MOCKABLE_FUNCTION(, MAP_RESULT, Map_AddOrUpdate, MAP_HANDLE, handle, const char*, key, const char*, value);

/**
 * @brief   Removes a key and its associated value from the map.
 *
 * @param   handle  The handle to an existing map.
 * @param   key     The @c key of the item to be deleted.
 *
 * @return  Returns @c MAP_OK if the key was deleted successfully or an
 *          error code otherwise.
 */
MOCKABLE_FUNCTION(, MAP_RESULT, Map_Delete, MAP_HANDLE, handle, const char*, key);

/**
 * @brief   This function returns a boolean value in @p keyExists if the map
 *          contains a key with the same value the parameter @p key.
 *
 * @param   handle      The handle to an existing map.
 * @param   key         The key that the caller wants checked.
 * @param   keyExists   The function writes @c true at the address pointed at
 *                      by this parameter if the key exists in the map and
 *                      @c false otherwise.
 *
 * @return  Returns @c MAP_OK if the check was performed successfully or an
 *          error code otherwise.
 */
MOCKABLE_FUNCTION(, MAP_RESULT, Map_ContainsKey, MAP_HANDLE, handle, const char*, key, bool*, keyExists);

/**
 * @brief   This function returns @c true in @p valueExists if at
 *          least one <key,value> pair exists in the map where the entry's
 *          value is equal to the parameter @c value.
 *
 * @param   handle          The handle to an existing map.
 * @param   value           The value that the caller wants checked.
 * @param   valueExists     The function writes @c true at the address pointed at
 *                          by this parameter if the value exists in the map and
 *                          @c false otherwise.
 *
 * @return  Returns @c MAP_OK if the check was performed successfully or an
 *          error code otherwise.
 */
MOCKABLE_FUNCTION(, MAP_RESULT, Map_ContainsValue, MAP_HANDLE, handle, const char*, value, bool*, valueExists);

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
MOCKABLE_FUNCTION(, const char*, Map_GetValueFromKey, MAP_HANDLE, handle, const char*, key);

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
 * @return  Returns @c MAP_OK if the keys and values are retrieved and written
 *          successfully or an error code otherwise.
 */
MOCKABLE_FUNCTION(, MAP_RESULT, Map_GetInternals, MAP_HANDLE, handle, const char*const**, keys, const char*const**, values, size_t*, count);

/*this API creates a JSON object from the content of the map*/
MOCKABLE_FUNCTION(, STRING_HANDLE, Map_ToJSON, MAP_HANDLE, handle);

#ifdef __cplusplus
}
#endif

#endif /* MAP_H */
