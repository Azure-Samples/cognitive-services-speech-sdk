// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include "azure_c_shared_utility/gballoc.h"
#include "azure_c_shared_utility/map.h"
#include "azure_c_shared_utility/xlogging.h"
#include "azure_c_shared_utility/strings.h"

DEFINE_ENUM_STRINGS(MAP_RESULT, MAP_RESULT_VALUES);

typedef struct MAP_HANDLE_DATA_TAG
{
    char** keys;
    char** values;
    size_t count;
    MAP_FILTER_CALLBACK mapFilterCallback;
}MAP_HANDLE_DATA;

#define LOG_MAP_ERROR LogError("result = %s", ENUM_TO_STRING(MAP_RESULT, result));

MAP_HANDLE Map_Create(MAP_FILTER_CALLBACK mapFilterFunc)
{
    /*Codes_SRS_MAP_02_001: [Map_Create shall create a new, empty map.]*/
    MAP_HANDLE_DATA* result = (MAP_HANDLE_DATA*)malloc(sizeof(MAP_HANDLE_DATA));
    /*Codes_SRS_MAP_02_002: [If during creation there are any error, then Map_Create shall return NULL.]*/
    if (result != NULL)
    {
        /*Codes_SRS_MAP_02_003: [Otherwise, it shall return a non-NULL handle that can be used in subsequent calls.] */
        result->keys = NULL;
        result->values = NULL;
        result->count = 0;
        result->mapFilterCallback = mapFilterFunc;
    }
    return (MAP_HANDLE)result;
}

void Map_Destroy(MAP_HANDLE handle)
{
    /*Codes_SRS_MAP_02_005: [If parameter handle is NULL then Map_Destroy shall take no action.] */
    if (handle != NULL)
    {
        /*Codes_SRS_MAP_02_004: [Map_Destroy shall release all resources associated with the map.] */
        MAP_HANDLE_DATA* handleData = (MAP_HANDLE_DATA*)handle;
        size_t i;
      
        for (i = 0; i < handleData->count; i++)
        {
            free(handleData->keys[i]);
            free(handleData->values[i]);
        }
        free(handleData->keys);
        free(handleData->values);
        free(handleData);
    }
}

/*makes a copy of a vector of const char*, having size "size". source cannot be NULL*/
/*returns NULL if it fails*/
static char** Map_CloneVector(const char*const * source, size_t count)
{
    char** result;
    result = (char**)malloc(count *sizeof(char*));
    if (result == NULL)
    {
        /*do nothing, just return it (NULL)*/
    }
    else
    {
        size_t i;
        for (i = 0; i < count; i++)
        {
            if (mallocAndStrcpy_s(result + i, source[i]) != 0)
            {
                break;
            }
        }

        if (i == count)
        {
            /*it is all good, proceed to return result*/
        }
        else
        {
            size_t j;
            for (j = 0; j < i; j++)
            {
                free(result[j]);
            }
            free(result);
            result = NULL;
        }
    }
    return result;
}

/*Codes_SRS_MAP_02_039: [Map_Clone shall make a copy of the map indicated by parameter handle and return a non-NULL handle to it.]*/
MAP_HANDLE Map_Clone(MAP_HANDLE handle)
{
    MAP_HANDLE_DATA* result;
    if (handle == NULL)
    {
        /*Codes_SRS_MAP_02_038: [Map_Clone returns NULL if parameter handle is NULL.]*/
        result = NULL;
        LogError("invalid arg to Map_Clone (NULL)");
    }
    else
    {
        MAP_HANDLE_DATA * handleData = (MAP_HANDLE_DATA *)handle;
        result = (MAP_HANDLE_DATA*)malloc(sizeof(MAP_HANDLE_DATA));
        if (result == NULL)
        {
            /*Codes_SRS_MAP_02_047: [If during cloning, any operation fails, then Map_Clone shall return NULL.] */
            /*do nothing, proceed to return it, this is an error case*/
            LogError("unable to malloc");
        }
        else
        {
            if (handleData->count == 0)  
            {
                result->count = 0;
                result->keys = NULL;
                result->values = NULL;
                result->mapFilterCallback = NULL;
            }
            else
            {
                result->mapFilterCallback = handleData->mapFilterCallback;
                result->count = handleData->count;
                if( (result->keys = Map_CloneVector((const char* const*)handleData->keys, handleData->count))==NULL)
                {
                    /*Codes_SRS_MAP_02_047: [If during cloning, any operation fails, then Map_Clone shall return NULL.] */
                    LogError("unable to clone keys");
                    free(result);
                    result = NULL;
                }
                else if ((result->values = Map_CloneVector((const char* const*)handleData->values, handleData->count)) == NULL)
                {
                    /*Codes_SRS_MAP_02_047: [If during cloning, any operation fails, then Map_Clone shall return NULL.] */
                    LogError("unable to clone values");
                    size_t i;
                    for (i = 0; i < result->count; i++)
                    {
                        free(result->keys[i]); 
                    }
                    free(result->keys);
                    free(result);
                    result = NULL;
                }
                else
                {
                    /*all fine, return it*/
                }
            }
        }
    }
    return (MAP_HANDLE)result;
}

static int Map_IncreaseStorageKeysValues(MAP_HANDLE_DATA* handleData)
{
    int result;
    char** newKeys = (char**)realloc(handleData->keys, (handleData->count + 1) * sizeof(char*));
    if (newKeys == NULL)
    {
        LogError("realloc error");
        result = __LINE__;
    }
    else
    {
        char** newValues;
        handleData->keys = newKeys;
        handleData->keys[handleData->count] = NULL;
        newValues = (char**)realloc(handleData->values, (handleData->count + 1) * sizeof(char*));
        if (newValues == NULL)
        {
            LogError("realloc error");
            if (handleData->count == 0) /*avoiding an implementation defined behavior */
            {
                free(handleData->keys);
                handleData->keys = NULL;
            }
            else
            {
                char** undoneKeys = (char**)realloc(handleData->keys, (handleData->count) * sizeof(char*));
                if (undoneKeys == NULL)
                {
                    LogError("CATASTROPHIC error, unable to undo through realloc to a smaller size");
                }
                else
                {
                    handleData->keys = undoneKeys;
                }
            }
            result = __LINE__;
        }
        else
        {
            handleData->values = newValues;
            handleData->values[handleData->count] = NULL;
            handleData->count++;
            result = 0;
        }
    }
    return result;
}

static void Map_DecreaseStorageKeysValues(MAP_HANDLE_DATA* handleData)
{
    if (handleData->count == 1)
    {
        free(handleData->keys);
        handleData->keys = NULL;
        free(handleData->values);
        handleData->values = NULL;
        handleData->count = 0;
        handleData->mapFilterCallback = NULL;
    }
    else
    {
        /*certainly > 1...*/
        char** undoneValues;
        char** undoneKeys = (char**)realloc(handleData->keys, sizeof(char*)* (handleData->count - 1)); 
        if (undoneKeys == NULL)
        {
            LogError("CATASTROPHIC error, unable to undo through realloc to a smaller size");
        }
        else
        {
            handleData->keys = undoneKeys;
        }

        undoneValues = (char**)realloc(handleData->values, sizeof(char*)* (handleData->count - 1));
        if (undoneValues == NULL)
        {
            LogError("CATASTROPHIC error, unable to undo through realloc to a smaller size");
        }
        else
        {
            handleData->values = undoneValues;
        }

        handleData->count--;
    }
}

static char** findKey(MAP_HANDLE_DATA* handleData, const char* key)
{
    char** result;
    if (handleData->keys == NULL)
    {
        result = NULL;
    }
    else
    {
        size_t i;
        result = NULL;
        for (i = 0; i < handleData->count; i++)
        {
            if (strcmp(handleData->keys[i], key) == 0)
            {
                result = handleData->keys + i;
                break;
            }
        }
    }
    return result;
}

static char** findValue(MAP_HANDLE_DATA* handleData, const char* value)
{
    char** result;
    if (handleData->values == NULL)
    {
        result = NULL;
    }
    else
    {
        size_t i;
        result = NULL;
        for (i = 0; i < handleData->count; i++)
        {
            if (strcmp(handleData->values[i], value) == 0)
            {
                result = handleData->values + i;
                break;
            }
        }
    }
    return result;
}

static int insertNewKeyValue(MAP_HANDLE_DATA* handleData, const char* key, const char* value)
{
    int result;
    if (Map_IncreaseStorageKeysValues(handleData) != 0) /*this increases handleData->count*/
    {
        result = __LINE__;
    }
    else
    {
        if (mallocAndStrcpy_s(&(handleData->keys[handleData->count - 1]), key) != 0)
        {
            Map_DecreaseStorageKeysValues(handleData);
            LogError("unable to mallocAndStrcpy_s");
            result = __LINE__;
        }
        else
        {
            if (mallocAndStrcpy_s(&(handleData->values[handleData->count - 1]), value) != 0)
            {
                free(handleData->keys[handleData->count - 1]);
                Map_DecreaseStorageKeysValues(handleData);
                LogError("unable to mallocAndStrcpy_s");
                result = __LINE__;
            }
            else
            {
                result = 0;
            }
        }
    }
    return result; 
}

MAP_RESULT Map_Add(MAP_HANDLE handle, const char* key, const char* value)
{
    MAP_RESULT result;
    /*Codes_SRS_MAP_02_006: [If parameter handle is NULL then Map_Add shall return MAP_INVALID_ARG.] */
    /*Codes_SRS_MAP_02_007: [If parameter key is NULL then Map_Add shall return MAP_INVALID_ARG.]*/
    /*Codes_SRS_MAP_02_008: [If parameter value is NULL then Map_Add shall return MAP_INVALID_ARG.] */
    if (
        (handle == NULL) ||
        (key == NULL) ||
        (value == NULL)
        )
    {
        result = MAP_INVALIDARG;
        LOG_MAP_ERROR; 
    }
    else
    {
        MAP_HANDLE_DATA* handleData = (MAP_HANDLE_DATA*)handle;
        /*Codes_SRS_MAP_02_009: [If the key already exists, then Map_Add shall return MAP_KEYEXISTS.] */
        if (findKey(handleData, key) != NULL)
        {
            result = MAP_KEYEXISTS;
        }
        else
        {
            /* Codes_SRS_MAP_07_009: [If the mapFilterCallback function is not NULL, then the return value will be check and if it is not zero then Map_Add shall return MAP_FILTER_REJECT.] */
            if ( (handleData->mapFilterCallback != NULL) && (handleData->mapFilterCallback(key, value) != 0) )
            {
                result = MAP_FILTER_REJECT;
            }
            else
            {
                /*Codes_SRS_MAP_02_010: [Otherwise, Map_Add shall add the pair <key,value> to the map.] */
                if (insertNewKeyValue(handleData, key, value) != 0)
                {
                    /*Codes_SRS_MAP_02_011: [If adding the pair <key,value> fails then Map_Add shall return MAP_ERROR.] */
                    result = MAP_ERROR;
                    LOG_MAP_ERROR;
                }
                else
                {
                    /*Codes_SRS_MAP_02_012: [Otherwise, Map_Add shall return MAP_OK.] */
                    result = MAP_OK;
                }
            }
        }
    }
    return result;
}

MAP_RESULT Map_AddOrUpdate(MAP_HANDLE handle, const char* key, const char* value)
{
    MAP_RESULT result;
    /*Codes_SRS_MAP_02_013: [If parameter handle is NULL then Map_AddOrUpdate shall return MAP_INVALID_ARG.]*/
    /*Codes_SRS_MAP_02_014: [If parameter key is NULL then Map_AddOrUpdate shall return MAP_INVALID_ARG.]*/
    /*Codes_SRS_MAP_02_015: [If parameter value is NULL then Map_AddOrUpdate shall return MAP_INVALID_ARG.] */
    if (
        (handle == NULL) ||
        (key == NULL) ||
        (value == NULL)
        )
    {
        result = MAP_INVALIDARG;
        LOG_MAP_ERROR;
    }
    else
    {
        MAP_HANDLE_DATA* handleData = (MAP_HANDLE_DATA*)handle;

        /* Codes_SRS_MAP_07_008: [If the mapFilterCallback function is not NULL, then the return value will be check and if it is not zero then Map_AddOrUpdate shall return MAP_FILTER_REJECT.] */
        if (handleData->mapFilterCallback != NULL && handleData->mapFilterCallback(key, value) != 0)
        {
            result = MAP_FILTER_REJECT;
        }
        else
        {
            char** whereIsIt = findKey(handleData, key);
            if (whereIsIt == NULL)
            {
                /*Codes_SRS_MAP_02_017: [Otherwise, Map_AddOrUpdate shall add the pair <key,value> to the map.]*/
                if (insertNewKeyValue(handleData, key, value) != 0)
                {
                    result = MAP_ERROR;
                    LOG_MAP_ERROR;
                }
                else
                {
                    result = MAP_OK;
                }
            }
            else
            {
                /*Codes_SRS_MAP_02_016: [If the key already exists, then Map_AddOrUpdate shall overwrite the value of the existing key with parameter value.]*/
                size_t index = whereIsIt - handleData->keys;
                size_t valueLength = strlen(value);
                /*try to realloc value of this key*/
                char* newValue = (char*)realloc(handleData->values[index],valueLength  + 1);
                if (newValue == NULL)
                {
                    result = MAP_ERROR;
                    LOG_MAP_ERROR;
                }
                else
                {
                    memcpy(newValue, value, valueLength + 1);
                    handleData->values[index] = newValue;
                    /*Codes_SRS_MAP_02_019: [Otherwise, Map_AddOrUpdate shall return MAP_OK.] */
                    result = MAP_OK;
                }
            }
        }
    }
    return result;
}

MAP_RESULT Map_Delete(MAP_HANDLE handle, const char* key)
{
    MAP_RESULT result;
    /*Codes_SRS_MAP_02_020: [If parameter handle is NULL then Map_Delete shall return MAP_INVALIDARG.]*/
    /*Codes_SRS_MAP_02_021: [If parameter key is NULL then Map_Delete shall return MAP_INVALIDARG.]*/
    if (
        (handle == NULL) ||
        (key == NULL)
        )
    {
        result = MAP_INVALIDARG;
        LOG_MAP_ERROR;
    }
    else
    {
        MAP_HANDLE_DATA* handleData = (MAP_HANDLE_DATA*)handle;
        char** whereIsIt = findKey(handleData,key);
        if (whereIsIt == NULL)
        {
            /*Codes_SRS_MAP_02_022: [If key does not exist then Map_Delete shall return MAP_KEYNOTFOUND.]*/
            result = MAP_KEYNOTFOUND;
        }
        else
        {
            /*Codes_SRS_MAP_02_023: [Otherwise, Map_Delete shall remove the key and its associated value from the map and return MAP_OK.]*/
            size_t index = whereIsIt - handleData->keys;
            free(handleData->keys[index]);
            free(handleData->values[index]);
            memmove(handleData->keys + index, handleData->keys + index + 1, (handleData->count - index - 1)*sizeof(char*)); /*if order doesn't matter... then this can be optimized*/
            memmove(handleData->values + index, handleData->values + index + 1, (handleData->count - index - 1)*sizeof(char*));
            Map_DecreaseStorageKeysValues(handleData);
            result = MAP_OK;
        }

    }
    return result;
}

MAP_RESULT Map_ContainsKey(MAP_HANDLE handle, const char* key, bool* keyExists)
{
    MAP_RESULT result;
    /*Codes_SRS_MAP_02_024: [If parameter handle, key or keyExists are NULL then Map_ContainsKey shall return MAP_INVALIDARG.]*/
    if (
        (handle ==NULL) ||
        (key == NULL) ||
        (keyExists == NULL)
        )
    {
        result = MAP_INVALIDARG;
        LOG_MAP_ERROR;
    }
    else
    {
        MAP_HANDLE_DATA* handleData = (MAP_HANDLE_DATA*)handle;
        /*Codes_SRS_MAP_02_025: [Otherwise if a key exists then Map_ContainsKey shall return MAP_OK and shall write in keyExists "true".]*/
        /*Codes_SRS_MAP_02_026: [If a key doesn't exist, then Map_ContainsKey shall return MAP_OK and write in keyExists "false".] */
        *keyExists = (findKey(handleData, key) != NULL) ? true: false;
        result = MAP_OK;
    }
    return result;
}

MAP_RESULT Map_ContainsValue(MAP_HANDLE handle, const char* value, bool* valueExists)
{
    MAP_RESULT result;
    /*Codes_SRS_MAP_02_027: [If parameter handle, value or valueExists is NULL then Map_ContainsValue shall return MAP_INVALIDARG.] */
    if (
        (handle == NULL) ||
        (value == NULL) ||
        (valueExists == NULL)
        )
    {
        result = MAP_INVALIDARG;
        LOG_MAP_ERROR;
    }
    else
    {
        MAP_HANDLE_DATA* handleData = (MAP_HANDLE_DATA*)handle;
        /*Codes_SRS_MAP_02_028: [Otherwise, if a pair <key, value> has its value equal to the parameter value, the Map_ContainsValue shall return MAP_OK and shall write in valueExists "true".]*/
        /*Codes_SRS_MAP_02_029: [Otherwise, if such a <key, value> does not exist, then Map_ContainsValue shall return MAP_OK and shall write in valueExists "false".] */
        *valueExists = (findValue(handleData, value) != NULL) ? true : false;
        result = MAP_OK;
    }
    return result;
}

const char* Map_GetValueFromKey(MAP_HANDLE handle, const char* key)
{
    const char* result;
    /*Codes_SRS_MAP_02_040: [If parameter handle or key is NULL then Map_GetValueFromKey returns NULL.]*/
    if (
        (handle == NULL) ||
        (key == NULL)
        )
    {
        result = NULL;
        LogError("invalid parameter to Map_GetValueFromKey");
    }
    else
    {
        MAP_HANDLE_DATA * handleData = (MAP_HANDLE_DATA *)handle;
        char** whereIsIt = findKey(handleData, key);
        if(whereIsIt == NULL)
        {
            /*Codes_SRS_MAP_02_041: [If the key is not found, then Map_GetValueFromKey returns NULL.]*/
            result = NULL;
        }
        else
        {
            /*Codes_SRS_MAP_02_042: [Otherwise, Map_GetValueFromKey returns the key's value.] */
            size_t index = whereIsIt - handleData->keys;
            result = handleData->values[index];
        }
    }
    return result;
}

MAP_RESULT Map_GetInternals(MAP_HANDLE handle, const char*const** keys, const char*const** values, size_t* count)
{
    MAP_RESULT result;
    /*Codes_SRS_MAP_02_046: [If parameter handle, keys, values or count is NULL then Map_GetInternals shall return MAP_INVALIDARG.] */
    if (
        (handle == NULL) ||
        (keys == NULL) ||
        (values == NULL) ||
        (count == NULL)
        )
    {
        result = MAP_INVALIDARG;
        LOG_MAP_ERROR;
    }
    else
    {
        /*Codes_SRS_MAP_02_043: [Map_GetInternals shall produce in *keys an pointer to an array of const char* having all the keys stored so far by the map.]*/
        /*Codes_SRS_MAP_02_044: [Map_GetInternals shall produce in *values a pointer to an array of const char* having all the values stored so far by the map.]*/
        /*Codes_SRS_MAP_02_045: [  Map_GetInternals shall produce in *count the number of stored keys and values.]*/
        MAP_HANDLE_DATA * handleData = (MAP_HANDLE_DATA *)handle;
        *keys =(const char* const*)(handleData->keys);
        *values = (const char* const*)(handleData->values);
        *count = handleData->count;
        result = MAP_OK;
    }
    return result;
}

STRING_HANDLE Map_ToJSON(MAP_HANDLE handle)
{
    STRING_HANDLE result;
    /*Codes_SRS_MAP_02_052: [If parameter handle is NULL then Map_ToJSON shall return NULL.] */
    if (handle == NULL)
    {
        result = NULL;
        LogError("invalid arg (NULL)");
    }
    else
    {
        /*Codes_SRS_MAP_02_048: [Map_ToJSON shall produce a STRING_HANDLE representing the content of the MAP.] */
        result = STRING_construct("{");
        if (result == NULL)
        {
            LogError("STRING_construct failed");
        }
        else
        {
            size_t i;
            MAP_HANDLE_DATA* handleData = (MAP_HANDLE_DATA *)handle;
            /*Codes_SRS_MAP_02_049: [If the MAP is empty, then Map_ToJSON shall produce the string "{}".*/
            bool breakFor = false; /*used to break out of for*/
            for (i = 0; (i < handleData->count) && (!breakFor); i++)
            {
                /*add one entry to the JSON*/
                /*Codes_SRS_MAP_02_050: [If the map has properties then Map_ToJSON shall produce the following string:{"name1":"value1", "name2":"value2" ...}]*/
                STRING_HANDLE key = STRING_new_JSON(handleData->keys[i]);
                if (key == NULL)
                {
                    LogError("STRING_new_JSON failed");
                    STRING_delete(result);
                    result = NULL;
                    breakFor = true;
                }
                else
                {
                    STRING_HANDLE value = STRING_new_JSON(handleData->values[i]);
                    if (value == NULL)
                    {
                        LogError("STRING_new_JSON failed");
                        STRING_delete(result);
                        result = NULL;
                        breakFor = true;
                    }
                    else
                    {
                        if (!(
                            ((i>0) ? (STRING_concat(result, ",") == 0) : 1) &&
                            (STRING_concat_with_STRING(result, key) == 0) &&
                            (STRING_concat(result, ":") == 0) &&
                            (STRING_concat_with_STRING(result, value) == 0)
                            ))
                        {
                            LogError("failed to build the JSON");
                            STRING_delete(result);
                            result = NULL;
                            breakFor = true;
                        }
                        else
                        {
                            /*all nice, go to the next element in the map*/
                        }
                        STRING_delete(value);
                    }
                    STRING_delete(key);
                }
            }
                
            if (breakFor)
            {
                LogError("error happened during JSON string builder");
            }
            else
            {
                if (STRING_concat(result, "}") != 0)
                {
                    LogError("failed to build the JSON");
                    STRING_delete(result);
                    result = NULL;
                }
                else
                {
                    /*return as is, JSON has been build*/
                }
            }
        }
    }
    return result;

}