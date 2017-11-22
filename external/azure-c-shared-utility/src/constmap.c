// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif
#include "azure_c_shared_utility/gballoc.h"

#include "azure_c_shared_utility/map.h"
#include "azure_c_shared_utility/constmap.h"
#include "azure_c_shared_utility/xlogging.h"
#include "azure_c_shared_utility/refcount.h"

DEFINE_ENUM_STRINGS(CONSTMAP_RESULT, CONSTMAP_RESULT_VALUES);

typedef struct CONSTMAP_HANDLE_DATA_TAG
{
    MAP_HANDLE map;
} CONSTMAP_HANDLE_DATA;

DEFINE_REFCOUNT_TYPE(CONSTMAP_HANDLE_DATA);

#define LOG_CONSTMAP_ERROR(result) LogError("result = %s", ENUM_TO_STRING(CONSTMAP_RESULT, (result)));

CONSTMAP_HANDLE ConstMap_Create(MAP_HANDLE sourceMap)
{
    CONSTMAP_HANDLE_DATA* result = REFCOUNT_TYPE_CREATE(CONSTMAP_HANDLE_DATA);

	if (result == NULL)
	{
		LOG_CONSTMAP_ERROR(CONSTMAP_INVALIDARG);
	}
	else
    {
		/*Codes_SRS_CONSTMAP_17_048: [ConstMap_Create shall accept any non-NULL MAP_HANDLE as input.]*/
		/*Codes_SRS_CONSTMAP_17_001: [ConstMap_Create shall create an immutable map, populated by the key, value pairs in the source map.]*/
        result->map = Map_Clone(sourceMap);
        if (result->map == NULL)
        {
            free(result);
			/*Codes_SRS_CONSTMAP_17_002: [If during creation there are any errors, then ConstMap_Create shall return NULL.]*/
            result = NULL;
			LOG_CONSTMAP_ERROR(CONSTMAP_ERROR);
        }

    }
	/*Codes_SRS_CONSTMAP_17_003: [Otherwise, it shall return a non-NULL handle that can be used in subsequent calls.]*/
    return (CONSTMAP_HANDLE)result;
}

void ConstMap_Destroy(CONSTMAP_HANDLE handle)
{
	/*Codes_SRS_CONSTMAP_17_005: [If parameter handle is NULL then ConstMap_Destroy shall take no action.]*/
	if (handle == NULL)
	{
		LOG_CONSTMAP_ERROR(CONSTMAP_INVALIDARG);
	}
	else
    {
		/*Codes_SRS_CONSTMAP_17_049: [ConstMap_Destroy shall decrement the internal reference count of the immutable map.]*/
		if (DEC_REF(CONSTMAP_HANDLE_DATA, handle) == DEC_RETURN_ZERO)
		{
			/*Codes_SRS_CONSTMAP_17_004: [If the reference count is zero, ConstMap_Destroy shall release all resources associated with the immutable map.]*/
			Map_Destroy(((CONSTMAP_HANDLE_DATA *)handle)->map);
			free(handle);
		}

    }
}

CONSTMAP_HANDLE ConstMap_Clone(CONSTMAP_HANDLE handle)
{
	/*Codes_SRS_CONSTMAP_17_038: [ConstMap_Clone returns NULL if parameter handle is NULL.] */
	if (handle == NULL)
	{
		LOG_CONSTMAP_ERROR(CONSTMAP_INVALIDARG);
	}
	else
    {
		/*Codes_SRS_CONSTMAP_17_039: [ConstMap_Clone shall increase the internal reference count of the immutable map indicated by parameter handle]*/
		/*Codes_SRS_CONSTMAP_17_050: [ConstMap_Clone shall return the non-NULL handle. ]*/
		INC_REF(CONSTMAP_HANDLE_DATA, handle);
    }
    return (handle);
}

static CONSTMAP_RESULT ConstMap_ErrorConvert(MAP_RESULT mapResult)
{
    CONSTMAP_RESULT result;
    switch (mapResult)
    {
        case MAP_OK:
            result = CONSTMAP_OK;
            break;
        case MAP_INVALIDARG:
            result = CONSTMAP_INVALIDARG;
            break;
        case MAP_KEYNOTFOUND:
            result = CONSTMAP_KEYNOTFOUND;
            break;
        default:
            result = CONSTMAP_ERROR;
            break;
    }
    return result;
}

MAP_HANDLE ConstMap_CloneWriteable(CONSTMAP_HANDLE handle)
{
	MAP_HANDLE result = NULL;
	if (handle == NULL)
	{
		/*Codes_SRS_CONSTMAP_17_051: [ConstMap_CloneWriteable returns NULL if parameter handle is NULL. ]*/
		LOG_CONSTMAP_ERROR(CONSTMAP_INVALIDARG);
	}
	else
	{
		/*Codes_SRS_CONSTMAP_17_052: [ConstMap_CloneWriteable shall create a new, writeable map, populated by the key, value pairs in the parameter defined by handle.]*/
		/*Codes_SRS_CONSTMAP_17_053: [If during cloning, any operation fails, then ConstMap_CloneWriteableap_Clone shall return NULL.]*/
		/*Codes_SRS_CONSTMAP_17_054: [Otherwise, ConstMap_CloneWriteable shall return a non-NULL handle that can be used in subsequent calls.]*/
		result = Map_Clone(((CONSTMAP_HANDLE_DATA *)handle)->map);
	}
	return result;
}

bool ConstMap_ContainsKey(CONSTMAP_HANDLE handle, const char* key )
{
	bool keyExists = false;
    if (handle == NULL)
    {
		/*Codes_SRS_CONSTMAP_17_024: [If parameter handle or key are NULL then ConstMap_ContainsKey shall return false.]*/
        LOG_CONSTMAP_ERROR(CONSTMAP_INVALIDARG);
    }
    else
    {
		if (key == NULL)
		{
			LOG_CONSTMAP_ERROR(CONSTMAP_INVALIDARG);
		}
		else
		{
			/*Codes_SRS_CONSTMAP_17_025: [Otherwise if a key exists then ConstMap_ContainsKey shall return true.]*/
			MAP_RESULT mapResult = Map_ContainsKey(((CONSTMAP_HANDLE_DATA *)handle)->map, key, &keyExists);
			if (mapResult != MAP_OK)
			{
				/*Codes_SRS_CONSTMAP_17_026: [If a key doesn't exist, then ConstMap_ContainsKey shall return false.]*/
				keyExists = false;
				LOG_CONSTMAP_ERROR(ConstMap_ErrorConvert(mapResult));
			}
		}
    }
    return keyExists;
}

bool ConstMap_ContainsValue(CONSTMAP_HANDLE handle, const char* value)
{
	bool valueExists = false;
    if (handle == NULL)
    {
		/*Codes_SRS_CONSTMAP_17_027: [If parameter handle or value is NULL then ConstMap_ContainsValue shall return false.]*/
        LOG_CONSTMAP_ERROR(CONSTMAP_INVALIDARG);
    }
    else
    {
		if (value == NULL)
		{
			LOG_CONSTMAP_ERROR(CONSTMAP_INVALIDARG);
		}
		else
		{
			/*Codes_SRS_CONSTMAP_17_028: [Otherwise, if a pair has its value equal to the parameter value, the ConstMap_ContainsValue shall return true.]*/
			MAP_RESULT mapResult = Map_ContainsValue(((CONSTMAP_HANDLE_DATA *)handle)->map, value, &valueExists);
			if (mapResult != MAP_OK)
			{
				/*Codes_SRS_CONSTMAP_17_029: [Otherwise, if such a does not exist, then ConstMap_ContainsValue shall return false.]*/
				LOG_CONSTMAP_ERROR(ConstMap_ErrorConvert(mapResult));
			}
		}
    }
    return valueExists;
}

const char* ConstMap_GetValue(CONSTMAP_HANDLE handle, const char* key)
{
	const char* value = NULL;
    
    if (handle == NULL)
    {
		/*Codes_SRS_CONSTMAP_17_040: [If parameter handle or key is NULL then ConstMap_GetValue returns NULL.]*/
		LOG_CONSTMAP_ERROR(CONSTMAP_INVALIDARG);
    }
    else
    {
		if (key == NULL)
		{
			/*Codes_SRS_CONSTMAP_17_040: [If parameter handle or key is NULL then ConstMap_GetValue returns NULL.]*/
			LOG_CONSTMAP_ERROR(CONSTMAP_INVALIDARG);
		}
		else
		{
			/*Codes_SRS_CONSTMAP_17_041: [If the key is not found, then ConstMap_GetValue returns NULL.]*/
			/*Codes_SRS_CONSTMAP_17_042: [Otherwise, ConstMap_GetValue returns the key's value.]*/
			value = Map_GetValueFromKey(((CONSTMAP_HANDLE_DATA *)handle)->map, key);
		}
    }
    return value;
}

CONSTMAP_RESULT ConstMap_GetInternals(CONSTMAP_HANDLE handle, const char*const** keys, const char*const** values, size_t* count)
{
    CONSTMAP_RESULT result;
    if (handle == NULL)
    {
		/*Codes_SRS_CONSTMAP_17_046: [If parameter handle, keys, values or count is NULL then ConstMap_GetInternals shall return CONSTMAP_INVALIDARG.]*/
        result = CONSTMAP_INVALIDARG;
        LOG_CONSTMAP_ERROR(result);
    }
    else
    {
		/*Codes_SRS_CONSTMAP_17_043: [ConstMap_GetInternals shall produce in *keys a pointer to an array of const char* having all the keys stored so far by the map.] 
		 *Codes_SRS_CONSTMAP_17_044: [ConstMap_GetInternals shall produce in *values a pointer to an array of const char* having all the values stored so far by the map.] 
		 *Codes_SRS_CONSTMAP_17_045: [ ConstMap_GetInternals shall produce in *count the number of stored keys and values.]
		 */
        MAP_RESULT mapResult = Map_GetInternals(((CONSTMAP_HANDLE_DATA *)handle)->map, keys, values, count);
        result = ConstMap_ErrorConvert(mapResult);
    }
    return result;
}