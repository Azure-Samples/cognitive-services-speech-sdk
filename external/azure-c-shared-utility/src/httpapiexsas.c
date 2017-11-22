// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif
#include "azure_c_shared_utility/gballoc.h"

#include <stddef.h>
#include <time.h>

#include "azure_c_shared_utility/agenttime.h"
#include "azure_c_shared_utility/strings.h"
#include "azure_c_shared_utility/buffer_.h"
#include "azure_c_shared_utility/sastoken.h"
#include "azure_c_shared_utility/httpheaders.h"
#include "azure_c_shared_utility/httpapiex.h"
#include "azure_c_shared_utility/httpapiexsas.h"
#include "azure_c_shared_utility/xlogging.h"

typedef struct HTTPAPIEX_SAS_STATE_TAG
{
    STRING_HANDLE key;
    STRING_HANDLE uriResource;
    STRING_HANDLE keyName;
}HTTPAPIEX_SAS_STATE;


HTTPAPIEX_SAS_HANDLE HTTPAPIEX_SAS_Create(STRING_HANDLE key, STRING_HANDLE uriResource, STRING_HANDLE keyName)
{
    HTTPAPIEX_SAS_HANDLE result = NULL;
    if (key == NULL)
    {
        /*Codes_SRS_HTTPAPIEXSAS_06_001: [If the parameter key is NULL then HTTPAPIEX_SAS_Create shall return NULL.]*/
        LogError("No key passed to HTTPAPIEX_SAS_Create.");
    }
    else if (uriResource == NULL)
    {
        /*Codes_SRS_HTTPAPIEXSAS_06_002: [If the parameter uriResource is NULL then HTTPAPIEX_SAS_Create shall return NULL.]*/
        LogError("No uri resource passed to HTTPAPIEX_SAS_Create.");
    }
    else if (keyName == NULL)
    {
        /*Codes_SRS_HTTPAPIEXSAS_06_003: [If the parameter keyName is NULL then HTTPAPIEX_SAS_Create shall return NULL.]*/
        LogError("No key name passed to HTTPAPIEX_SAS_Create.");
    }
    else
    {
        HTTPAPIEX_SAS_STATE* state = malloc(sizeof(HTTPAPIEX_SAS_STATE));
        /*Codes_SRS_HTTPAPIEXSAS_06_004: [If there are any other errors in the instantiation of this handle then HTTPAPIEX_SAS_Create shall return NULL.]*/
        if (state != NULL)
        {
            state->key = NULL;
            state->uriResource = NULL;
            state->keyName = NULL;
            if (((state->key = STRING_clone(key)) == NULL) ||
                ((state->uriResource = STRING_clone(uriResource)) == NULL) ||
                ((state->keyName = STRING_clone(keyName)) == NULL))
            {
                /*Codes_SRS_HTTPAPIEXSAS_06_004: [If there are any other errors in the instantiation of this handle then HTTPAPIEX_SAS_Create shall return NULL.]*/
                LogError("Unable to clone the arguments.");
                HTTPAPIEX_SAS_Destroy(state);
            }
            else
            {
                result = state;
            }
        }
    }
    return result;
}

void HTTPAPIEX_SAS_Destroy(HTTPAPIEX_SAS_HANDLE handle)
{
    /*Codes_SRS_HTTPAPIEXSAS_06_005: [If the parameter handle is NULL then HTTAPIEX_SAS_Destroy shall do nothing and return.]*/
    if (handle)
    {
        HTTPAPIEX_SAS_STATE* state = (HTTPAPIEX_SAS_STATE*)handle;
        /*Codes_SRS_HTTPAPIEXSAS_06_006: [HTTAPIEX_SAS_Destroy shall deallocate any structures denoted by the parameter handle.]*/
        if (state->key)
        {
            STRING_delete(state->key);
        }
        if (state->uriResource)
        {
            STRING_delete(state->uriResource);
        }
        if (state->keyName)
        {
            STRING_delete(state->keyName);
        }
        free(state);
    }
}

HTTPAPIEX_RESULT HTTPAPIEX_SAS_ExecuteRequest(HTTPAPIEX_SAS_HANDLE sasHandle, HTTPAPIEX_HANDLE handle, HTTPAPI_REQUEST_TYPE requestType, const char* relativePath, HTTP_HEADERS_HANDLE requestHttpHeadersHandle, BUFFER_HANDLE requestContent, unsigned int* statusCode, HTTP_HEADERS_HANDLE responseHeadersHandle, BUFFER_HANDLE responseContent)
{
    /*Codes_SRS_HTTPAPIEXSAS_06_007: [If the parameter sasHandle is NULL then HTTPAPIEX_SAS_ExecuteRequest shall simply invoke HTTPAPIEX_ExecuteRequest with the remaining parameters (following sasHandle) as its arguments and shall return immediately with the result of that call as the result of HTTPAPIEX_SAS_ExecuteRequest.]*/
    if (sasHandle != NULL)
    {
        /*Codes_SRS_HTTPAPIEXSAS_06_008: [if the parameter requestHttpHeadersHandle is NULL then fallthrough.]*/
        if (requestHttpHeadersHandle != NULL)
        {
            /*Codes_SRS_HTTPAPIEXSAS_06_009: [HTTPHeaders_FindHeaderValue shall be invoked with the requestHttpHeadersHandle as its first argument and the string "Authorization" as its second argument.]*/
            /*Codes_SRS_HTTPAPIEXSAS_06_010: [If the return result of the invocation of HTTPHeaders_FindHeaderValue is NULL then fallthrough.]*/
            if (HTTPHeaders_FindHeaderValue(requestHttpHeadersHandle, "Authorization") != NULL)
            {
                HTTPAPIEX_SAS_STATE* state = (HTTPAPIEX_SAS_STATE*)sasHandle;
                /*Codes_SRS_HTTPAPIEXSAS_06_018: [A value of type time_t that shall be known as currentTime is obtained from calling get_time.]*/
                time_t currentTime = get_time(NULL);
                /*Codes_SRS_HTTPAPIEXSAS_06_019: [If the value of currentTime is (time_t)-1 is then fallthrough.]*/
                if (currentTime == (time_t)-1)
                {
                    LogError("Time does not appear to be working.");
                }
                else
                {
                    /*Codes_SRS_HTTPAPIEXSAS_06_011: [SASToken_Create shall be invoked.]*/
                    /*Codes_SRS_HTTPAPIEXSAS_06_012: [If the return result of SASToken_Create is NULL then fallthrough.]*/
                    size_t expiry = (size_t)(difftime(currentTime, 0) + 3600);
                    STRING_HANDLE newSASToken = SASToken_Create(state->key, state->uriResource, state->keyName, expiry);
                    if (newSASToken != NULL)
                    {
                        /*Codes_SRS_HTTPAPIEXSAS_06_013: [HTTPHeaders_ReplaceHeaderNameValuePair shall be invoked with "Authorization" as its second argument and STRING_c_str (newSASToken) as its third argument.]*/
                        if (HTTPHeaders_ReplaceHeaderNameValuePair(requestHttpHeadersHandle, "Authorization", STRING_c_str(newSASToken)) != HTTP_HEADERS_OK)
                        {
                            /*Codes_SRS_HTTPAPIEXSAS_06_014: [If the result of the invocation of HTTPHeaders_ReplaceHeaderNameValuePair is NOT HTTP_HEADERS_OK then fallthrough.]*/
                            LogError("Unable to replace the old SAS Token.");
                        }
                        /*Codes_SRS_HTTPAPIEXSAS_06_015: [STRING_delete(newSASToken) will be invoked.]*/
                        STRING_delete(newSASToken);
                    }
                    else
                    {
                        LogError("Unable to create a new SAS token.");
                    }
                }
            }
        }
    }
    /*Codes_SRS_HTTPAPIEXSAS_06_016: [HTTPAPIEX_ExecuteRequest with the remaining parameters (following sasHandle) as its arguments will be invoked and the result of that call is the result of HTTPAPIEX_SAS_ExecuteRequest.]*/
    return HTTPAPIEX_ExecuteRequest(handle,requestType,relativePath,requestHttpHeadersHandle,requestContent,statusCode,responseHeadersHandle,responseContent);
}
