// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif
#include "azure_c_shared_utility/gballoc.h"

#include "azure_c_shared_utility/map.h"
#include "azure_c_shared_utility/httpheaders.h"
#include <string.h>
#include "azure_c_shared_utility/crt_abstractions.h"
#include "azure_c_shared_utility/xlogging.h"

static const char COLON_AND_SPACE[] = { ':', ' ', '\0' };
#define COLON_AND_SPACE_LENGTH  ((sizeof(COLON_AND_SPACE)/sizeof(COLON_AND_SPACE[0]))-1)

static const char COMMA_AND_SPACE[] = { ',', ' ', '\0' };
#define COMMA_AND_SPACE_LENGTH  ((sizeof(COMMA_AND_SPACE)/sizeof(COMMA_AND_SPACE[0]))-1)

DEFINE_ENUM_STRINGS(HTTP_HEADERS_RESULT, HTTP_HEADERS_RESULT_VALUES);

typedef struct HTTP_HEADERS_HANDLE_DATA_TAG
{
    MAP_HANDLE headers;
} HTTP_HEADERS_HANDLE_DATA;

HTTP_HEADERS_HANDLE HTTPHeaders_Alloc(void)
{
    /*Codes_SRS_HTTP_HEADERS_99_002:[ This API shall produce a HTTP_HANDLE that can later be used in subsequent calls to the module.]*/
    HTTP_HEADERS_HANDLE_DATA* result;
    result = (HTTP_HEADERS_HANDLE_DATA*)malloc(sizeof(HTTP_HEADERS_HANDLE_DATA));

    if (result == NULL)
    {
        LogError("malloc failed");
    }
    else
    {
        /*Codes_SRS_HTTP_HEADERS_99_004:[ After a successful init, HTTPHeaders_GetHeaderCount shall report 0 existing headers.]*/
        result->headers = Map_Create(NULL);
        if (result->headers == NULL)
        {
            LogError("Map_Create failed");
            free(result);
            result = NULL;
        }
        else
        {
            /*all is fine*/
        }
    }

    /*Codes_SRS_HTTP_HEADERS_99_003:[ The function shall return NULL when the function cannot execute properly]*/
    return (HTTP_HEADERS_HANDLE)result;
}

/*Codes_SRS_HTTP_HEADERS_99_005:[ Calling this API shall de-allocate the data structures allocated by previous API calls to the same handle.]*/
void HTTPHeaders_Free(HTTP_HEADERS_HANDLE handle)
{
    /*Codes_SRS_HTTP_HEADERS_02_001: [If httpHeadersHandle is NULL then HTTPHeaders_Free shall perform no action.] */
    if (handle == NULL)
    {
        /*do nothing*/
    }
    else
    {
        /*Codes_SRS_HTTP_HEADERS_99_005:[ Calling this API shall de-allocate the data structures allocated by previous API calls to the same handle.]*/
        HTTP_HEADERS_HANDLE_DATA* handleData = (HTTP_HEADERS_HANDLE_DATA*)handle;

        Map_Destroy(handleData->headers);
        free(handleData);
    }
}

/*Codes_SRS_HTTP_HEADERS_99_012:[ Calling this API shall record a header from name and value parameters.]*/
static HTTP_HEADERS_RESULT headers_ReplaceHeaderNameValuePair(HTTP_HEADERS_HANDLE handle, const char* name, const char* value, bool replace)
{
    HTTP_HEADERS_RESULT result;
    /*Codes_SRS_HTTP_HEADERS_99_014:[ The function shall return when the handle is not valid or when name parameter is NULL or when value parameter is NULL.]*/
    if (
        (handle == NULL) ||
        (name == NULL) ||
        (value == NULL)
        )
    {
        result = HTTP_HEADERS_INVALID_ARG;
        LogError("invalid arg (NULL) , result= %s", ENUM_TO_STRING(HTTP_HEADERS_RESULT, result));
    }
    else
    {
        /*Codes_SRS_HTTP_HEADERS_99_036:[ If name contains the characters outside character codes 33 to 126 then the return value shall be HTTP_HEADERS_INVALID_ARG]*/
        /*Codes_SRS_HTTP_HEADERS_99_031:[ If name contains the character ":" then the return value shall be HTTP_HEADERS_INVALID_ARG.]*/
        size_t i;
        size_t nameLen = strlen(name);
        for (i = 0; i < nameLen; i++)
        {
            if ((name[i] < 33) || (126 < name[i]) || (name[i] == ':'))
            {
                break;
            }
        }

        if (i < nameLen)
        {
            result = HTTP_HEADERS_INVALID_ARG;
            LogErrorResult(HTTP_HEADERS_RESULT, result);
        }
        else
        {
            HTTP_HEADERS_HANDLE_DATA* handleData = (HTTP_HEADERS_HANDLE_DATA*)handle;
            const char* existingValue = Map_GetValueFromKey(handleData->headers, name);
            /*eat up the whitespaces from value, as per RFC 2616, chapter 4.2 "The field value MAY be preceded by any amount of LWS, though a single SP is preferred."*/
            /*Codes_SRS_HTTP_HEADERS_02_002: [The LWS from the beginning of the value shall not be stored.] */
            while ((value[0] == ' ') || (value[0] == '\t') || (value[0] == '\r') || (value[0] == '\n'))
            {
                value++;
            }

            if (!replace && (existingValue != NULL))
            {
				size_t newValueSize = strlen(existingValue) + COMMA_AND_SPACE_LENGTH + strlen(value) + 1;
                char* newValue = (char*)malloc(newValueSize);
                if (newValue == NULL)
                {
                    /*Codes_SRS_HTTP_HEADERS_99_015:[ The function shall return HTTP_HEADERS_ALLOC_FAILED when an internal request to allocate memory fails.]*/
                    result = HTTP_HEADERS_ALLOC_FAILED;
                    LogError("failed to malloc , result= %s", ENUM_TO_STRING(HTTP_HEADERS_RESULT, result));
                }
                else
                {
                    /*Codes_SRS_HTTP_HEADERS_99_017:[ If the name already exists in the collection of headers, the function shall concatenate the new value after the existing value, separated by a comma and a space as in: old-value+", "+new-value.]*/
                    (void)strcpy_s(newValue, newValueSize, existingValue);
                    (void)strcat_s(newValue, newValueSize, COMMA_AND_SPACE);
                    (void)strcat_s(newValue, newValueSize, value);

                    /*Codes_SRS_HTTP_HEADERS_99_016:[ The function shall store the name:value pair in such a way that when later retrieved by a call to GetHeader it will return a string that shall strcmp equal to the name+": "+value.]*/
                    if (Map_AddOrUpdate(handleData->headers, name, newValue) != MAP_OK)
                    {
                        /*Codes_SRS_HTTP_HEADERS_99_015:[ The function shall return HTTP_HEADERS_ALLOC_FAILED when an internal request to allocate memory fails.]*/
                        result = HTTP_HEADERS_ERROR;
                        LogError("failed to Map_AddOrUpdate, result= %s", ENUM_TO_STRING(HTTP_HEADERS_RESULT, result));
                    }
                    else
                    {
                        /*Codes_SRS_HTTP_HEADERS_99_013:[ The function shall return HTTP_HEADERS_OK when execution is successful.]*/
                        result = HTTP_HEADERS_OK;
                    }
                    free(newValue);
                }
            }
            else
            {
                /*Codes_SRS_HTTP_HEADERS_99_016:[ The function shall store the name:value pair in such a way that when later retrieved by a call to GetHeader it will return a string that shall strcmp equal to the name+": "+value.]*/
                if (Map_AddOrUpdate(handleData->headers, name, value) != MAP_OK)
                {
                    /*Codes_SRS_HTTP_HEADERS_99_015:[ The function shall return HTTP_HEADERS_ALLOC_FAILED when an internal request to allocate memory fails.]*/
                    result = HTTP_HEADERS_ALLOC_FAILED;
                    LogError("failed to Map_AddOrUpdate, result= %s", ENUM_TO_STRING(HTTP_HEADERS_RESULT, result));
                }
                else
                {
                    result = HTTP_HEADERS_OK;
                }
            }
        }
    }

    return result;
}

HTTP_HEADERS_RESULT HTTPHeaders_AddHeaderNameValuePair(HTTP_HEADERS_HANDLE httpHeadersHandle, const char* name, const char* value)
{
    return headers_ReplaceHeaderNameValuePair(httpHeadersHandle, name, value, false);
}

/* Codes_SRS_HTTP_HEADERS_06_001: [This API will perform exactly as HTTPHeaders_AddHeaderNameValuePair except that if the header name already exists the already existing value will be replaced as opposed to concatenated to.] */
HTTP_HEADERS_RESULT HTTPHeaders_ReplaceHeaderNameValuePair(HTTP_HEADERS_HANDLE httpHeadersHandle, const char* name, const char* value)
{
    return headers_ReplaceHeaderNameValuePair(httpHeadersHandle, name, value, true);
}


const char* HTTPHeaders_FindHeaderValue(HTTP_HEADERS_HANDLE httpHeadersHandle, const char* name)
{
    const char* result;
    /*Codes_SRS_HTTP_HEADERS_99_022:[ The return value shall be NULL if name parameter is NULL or if httpHeadersHandle is NULL]*/
    if (
        (httpHeadersHandle == NULL) ||
        (name == NULL)
        )
    {
        result = NULL;
    }
    else
    {
        /*Codes_SRS_HTTP_HEADERS_99_018:[ Calling this API shall retrieve the value for a previously stored name.]*/
        /*Codes_SRS_HTTP_HEADERS_99_020:[ The return value shall be different than NULL when the name matches the name of a previously stored name:value pair.] */
        /*Codes_SRS_HTTP_HEADERS_99_021:[ In this case the return value shall point to a string that shall strcmp equal to the original stored string.]*/
        HTTP_HEADERS_HANDLE_DATA* handleData = (HTTP_HEADERS_HANDLE_DATA*)httpHeadersHandle;
        result = Map_GetValueFromKey(handleData->headers, name);
    }
    return result;

}

HTTP_HEADERS_RESULT HTTPHeaders_GetHeaderCount(HTTP_HEADERS_HANDLE handle, size_t* headerCount)
{
    HTTP_HEADERS_RESULT result;
    /*Codes_SRS_HTTP_HEADERS_99_024:[ The function shall return HTTP_HEADERS_INVALID_ARG when an invalid handle is passed.]*/
    /*Codes_SRS_HTTP_HEADERS_99_025:[ The function shall return HTTP_HEADERS_INVALID_ARG when headersCount is NULL.]*/
    if ((handle == NULL) ||
        (headerCount == NULL))
    {
        result = HTTP_HEADERS_INVALID_ARG;
        LogErrorResult(HTTP_HEADERS_RESULT, result);
    }
    else
    {
        HTTP_HEADERS_HANDLE_DATA *handleData = (HTTP_HEADERS_HANDLE_DATA *)handle;
        const char*const* keys;
        const char*const* values;
        /*Codes_SRS_HTTP_HEADERS_99_023:[ Calling this API shall provide the number of stored headers.]*/
        if (Map_GetInternals(handleData->headers, &keys, &values, headerCount) != MAP_OK)
        {
            /*Codes_SRS_HTTP_HEADERS_99_037:[ The function shall return HTTP_HEADERS_ERROR when an internal error occurs.]*/
            result = HTTP_HEADERS_ERROR;
            LogError("Map_GetInternals failed, result= %s", ENUM_TO_STRING(HTTP_HEADERS_RESULT, result));
        }
        else
        {
            /*Codes_SRS_HTTP_HEADERS_99_026:[ The function shall write in *headersCount the number of currently stored headers and shall return HTTP_HEADERS_OK]*/
            result = HTTP_HEADERS_OK;
        }
    }

    return result;
}

/*produces a string in *destination that is equal to name: value*/
HTTP_HEADERS_RESULT HTTPHeaders_GetHeader(HTTP_HEADERS_HANDLE handle, size_t index, char** destination)
{
    HTTP_HEADERS_RESULT result = HTTP_HEADERS_OK;

    /*Codes_SRS_HTTP_HEADERS_99_028:[ The function shall return NULL if the handle is invalid.]*/
    /*Codes_SRS_HTTP_HEADERS_99_032:[ The function shall return HTTP_HEADERS_INVALID_ARG if the destination  is NULL]*/
    if (
        (handle == NULL) ||
        (destination == NULL)
        )
    {
        result = HTTP_HEADERS_INVALID_ARG;
        LogError("invalid arg (NULL), result= %s", ENUM_TO_STRING(HTTP_HEADERS_RESULT, result));
    }
    /*Codes_SRS_HTTP_HEADERS_99_029:[ The function shall return HTTP_HEADERS_INVALID_ARG if index is not valid (for example, out of range) for the currently stored headers.]*/
    else
    {
        HTTP_HEADERS_HANDLE_DATA* handleData = (HTTP_HEADERS_HANDLE_DATA*)handle;
        const char*const* keys;
        const char*const* values;
        size_t headerCount;
        if (Map_GetInternals(handleData->headers, &keys, &values, &headerCount) != MAP_OK)
        {
            /*Codes_SRS_HTTP_HEADERS_99_034:[ The function shall return HTTP_HEADERS_ERROR when an internal error occurs]*/
            result = HTTP_HEADERS_ERROR;
            LogError("Map_GetInternals failed, result= %s", ENUM_TO_STRING(HTTP_HEADERS_RESULT, result));
        }
        else
        {
            /*Codes_SRS_HTTP_HEADERS_99_029:[ The function shall return HTTP_HEADERS_INVALID_ARG if index is not valid (for example, out of range) for the currently stored headers.]*/
            if (index >= headerCount)
            {
                result = HTTP_HEADERS_INVALID_ARG;
                LogError("index out of bounds, result= %s", ENUM_TO_STRING(HTTP_HEADERS_RESULT, result));
            }
            else
            {
				size_t destinationSize = strlen(keys[index]) + COLON_AND_SPACE_LENGTH + strlen(values[index]) + 1;
                *destination = (char*)malloc(destinationSize);
                if (*destination == NULL)
                {
                    /*Codes_SRS_HTTP_HEADERS_99_034:[ The function shall return HTTP_HEADERS_ERROR when an internal error occurs]*/
                    result = HTTP_HEADERS_ERROR;
                    LogError("unable to malloc, result= %s", ENUM_TO_STRING(HTTP_HEADERS_RESULT, result));
                }
                else
                {
                    /*Codes_SRS_HTTP_HEADERS_99_016:[ The function shall store the name:value pair in such a way that when later retrieved by a call to GetHeader it will return a string that shall strcmp equal to the name+": "+value.]*/
                    /*Codes_SRS_HTTP_HEADERS_99_027:[ Calling this API shall produce the string value+": "+pair) for the index header in the *destination parameter.]*/
                    strcpy_s(*destination, destinationSize, keys[index]);
                    strcat_s(*destination, destinationSize, COLON_AND_SPACE);
                    strcat_s(*destination, destinationSize, values[index]);
                    /*Codes_SRS_HTTP_HEADERS_99_035:[ The function shall return HTTP_HEADERS_OK when the function executed without error.]*/
                    result = HTTP_HEADERS_OK;
                }
            }
        }
    }

    return result;
}

HTTP_HEADERS_HANDLE HTTPHeaders_Clone(HTTP_HEADERS_HANDLE handle)
{
    HTTP_HEADERS_HANDLE_DATA* result;
    /*Codes_SRS_HTTP_HEADERS_02_003: [If handle is NULL then HTTPHeaders_Clone shall return NULL.] */
    if (handle == NULL)
    {
        result = NULL;
    }
    else
    {
        /*Codes_SRS_HTTP_HEADERS_02_004: [Otherwise HTTPHeaders_Clone shall clone the content of handle to a new handle.] */
        result = malloc(sizeof(HTTP_HEADERS_HANDLE_DATA));
        if (result == NULL)
        {
            /*Codes_SRS_HTTP_HEADERS_02_005: [If cloning fails for any reason, then HTTPHeaders_Clone shall return NULL.] */
        }
        else
        {
            HTTP_HEADERS_HANDLE_DATA* handleData = handle;
            result->headers = Map_Clone(handleData->headers);
            if (result->headers == NULL)
            {
                /*Codes_SRS_HTTP_HEADERS_02_005: [If cloning fails for any reason, then HTTPHeaders_Clone shall return NULL.] */
                free(result);
                result = NULL;
            }
            else
            {
                /*all is fine*/
            }
        }
    }
    return result;
}
