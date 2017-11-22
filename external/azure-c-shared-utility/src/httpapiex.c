// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif
#include "azure_c_shared_utility/gballoc.h"

#include "azure_c_shared_utility/httpapiex.h"
#include "azure_c_shared_utility/xlogging.h"
#include "azure_c_shared_utility/strings.h"
#include "azure_c_shared_utility/crt_abstractions.h"
#include "azure_c_shared_utility/vector.h"

typedef struct HTTPAPIEX_SAVED_OPTION_TAG
{
    const char* optionName;
    const void* value;
}HTTPAPIEX_SAVED_OPTION;

typedef struct HTTPAPIEX_HANDLE_DATA_TAG
{
    STRING_HANDLE hostName;
    int k;
    HTTP_HANDLE httpHandle;
    VECTOR_HANDLE savedOptions;
}HTTPAPIEX_HANDLE_DATA;

DEFINE_ENUM_STRINGS(HTTPAPIEX_RESULT, HTTPAPIEX_RESULT_VALUES);

#define LOG_HTTAPIEX_ERROR() LogError("error code = %s", ENUM_TO_STRING(HTTPAPIEX_RESULT, result))

HTTPAPIEX_HANDLE HTTPAPIEX_Create(const char* hostName)
{
    HTTPAPIEX_HANDLE result;
    /*Codes_SRS_HTTPAPIEX_02_001: [If parameter hostName is NULL then HTTPAPIEX_Create shall return NULL.]*/
    if (hostName == NULL)
    {
        LogError("invalid (NULL) parameter");
        result = NULL;
    }
    else
    {
        /*Codes_SRS_HTTPAPIEX_02_005: [If creating the handle fails for any reason, then HTTAPIEX_Create shall return NULL.] */
        HTTPAPIEX_HANDLE_DATA* handleData = (HTTPAPIEX_HANDLE_DATA*)malloc(sizeof(HTTPAPIEX_HANDLE_DATA));
        if (handleData == NULL)
        {
            LogError("malloc failed.");
            result = NULL;
        }
        else
        {
            /*Codes_SRS_HTTPAPIEX_02_002: [Parameter hostName shall be saved.]*/
            handleData->hostName = STRING_construct(hostName);
            if (handleData->hostName == NULL)
            {
                free(handleData);
                LogError("unable to STRING_construct");
                result = NULL;
            }
            else
            {
                /*Codes_SRS_HTTPAPIEX_02_004: [Otherwise, HTTPAPIEX_Create shall return a HTTAPIEX_HANDLE suitable for further calls to the module.] */
                handleData->savedOptions = VECTOR_create(sizeof(HTTPAPIEX_SAVED_OPTION));
                if (handleData->savedOptions == NULL)
                {
                    STRING_delete(handleData->hostName);
                    free(handleData);
                    result = NULL;
                }
                else
                {
                    handleData->k = -1;
                    handleData->httpHandle = NULL;
                    result = handleData;
                }
            }
        }
    }
    return result;
}

/*this function builds the default request http headers if none are specified*/
/*returns 0 if no error*/
/*any other code is error*/
static int buildRequestHttpHeadersHandle(HTTPAPIEX_HANDLE_DATA *handleData, BUFFER_HANDLE requestContent, HTTP_HEADERS_HANDLE originalRequestHttpHeadersHandle, bool* isOriginalRequestHttpHeadersHandle, HTTP_HEADERS_HANDLE* toBeUsedRequestHttpHeadersHandle)
{
    int result;


    if (originalRequestHttpHeadersHandle != NULL)
    {
        *toBeUsedRequestHttpHeadersHandle = originalRequestHttpHeadersHandle;
        *isOriginalRequestHttpHeadersHandle = true;

    }
    else
    {
        /*Codes_SRS_HTTPAPIEX_02_009: [If parameter requestHttpHeadersHandle is NULL then HTTPAPIEX_ExecuteRequest shall allocate a temporary internal instance of HTTPHEADERS, shall add to that instance the following headers
        Host:{hostname} - as it was indicated by the call to HTTPAPIEX_Create API call
        Content-Length:the size of the requestContent parameter, and use this instance to all the subsequent calls to HTTPAPI_ExecuteRequest as parameter httpHeadersHandle.]
        */
        *isOriginalRequestHttpHeadersHandle = false;
        *toBeUsedRequestHttpHeadersHandle = HTTPHeaders_Alloc();
    }

    if (*toBeUsedRequestHttpHeadersHandle == NULL)
    {
        result = __LINE__;
        LogError("unable to HTTPHeaders_Alloc");
    }
    else
    {
        char temp[22] = { 0 };
        (void)size_tToString(temp, 22, BUFFER_length(requestContent)); /*cannot fail, MAX_uint64 has 19 digits*/
        /*Codes_SRS_HTTPAPIEX_02_011: [If parameter requestHttpHeadersHandle is not NULL then HTTPAPIEX_ExecuteRequest shall create or update the following headers of the request:
        Host:{hostname}
        Content-Length:the size of the requestContent parameter, and shall use the so constructed HTTPHEADERS object to all calls to HTTPAPI_ExecuteRequest as parameter httpHeadersHandle.]
        */
        /*Codes_SRS_HTTPAPIEX_02_009: [If parameter requestHttpHeadersHandle is NULL then HTTPAPIEX_ExecuteRequest shall allocate a temporary internal instance of HTTPHEADERS, shall add to that instance the following headers
        Host:{hostname} - as it was indicated by the call to HTTPAPIEX_Create API call
        Content-Length:the size of the requestContent parameter, and use this instance to all the subsequent calls to HTTPAPI_ExecuteRequest as parameter httpHeadersHandle.]
        */
        if (!(
            (HTTPHeaders_ReplaceHeaderNameValuePair(*toBeUsedRequestHttpHeadersHandle, "Host", STRING_c_str(handleData->hostName)) == HTTP_HEADERS_OK) &&
            (HTTPHeaders_ReplaceHeaderNameValuePair(*toBeUsedRequestHttpHeadersHandle, "Content-Length", temp) == HTTP_HEADERS_OK)
            ))
        {
            if (! *isOriginalRequestHttpHeadersHandle)
            { 
                HTTPHeaders_Free(*toBeUsedRequestHttpHeadersHandle);
            }
            *toBeUsedRequestHttpHeadersHandle = NULL;
            result = __LINE__;
        }
        else
        {
            result = 0;
        }
    }
    return result;
}

static int buildResponseHttpHeadersHandle(HTTP_HEADERS_HANDLE originalResponsetHttpHeadersHandle, bool* isOriginalResponseHttpHeadersHandle, HTTP_HEADERS_HANDLE* toBeUsedResponsetHttpHeadersHandle)
{
    int result;
    if (originalResponsetHttpHeadersHandle == NULL)
    {
        if ((*toBeUsedResponsetHttpHeadersHandle = HTTPHeaders_Alloc()) == NULL)
        {
            result = __LINE__;
        }
        else
        {
            *isOriginalResponseHttpHeadersHandle = false;
            result = 0;
        }
    }
    else
    {
        *isOriginalResponseHttpHeadersHandle = true;
        *toBeUsedResponsetHttpHeadersHandle = originalResponsetHttpHeadersHandle;
        result = 0;
    }
    return result;
}


static int buildBufferIfNotExist(BUFFER_HANDLE originalRequestContent, bool* isOriginalRequestContent, BUFFER_HANDLE* toBeUsedRequestContent)
{
    int result;
    if (originalRequestContent == NULL)
    {
        *toBeUsedRequestContent = BUFFER_new();
        if (*toBeUsedRequestContent == NULL)
        {
            result = __LINE__;
        }
        else
        {
            *isOriginalRequestContent = false;
            result = 0;
        }
    }
    else
    {
        *isOriginalRequestContent = true;
        *toBeUsedRequestContent = originalRequestContent;
        result = 0;
    }
    return result;
}

static unsigned int dummyStatusCode;

static int buildAllRequests(HTTPAPIEX_HANDLE_DATA* handle, HTTPAPI_REQUEST_TYPE requestType, const char* relativePath,
    HTTP_HEADERS_HANDLE requestHttpHeadersHandle, BUFFER_HANDLE requestContent, unsigned int* statusCode,
    HTTP_HEADERS_HANDLE responseHttpHeadersHandle, BUFFER_HANDLE responseContent,

    const char** toBeUsedRelativePath, 
    HTTP_HEADERS_HANDLE *toBeUsedRequestHttpHeadersHandle, bool *isOriginalRequestHttpHeadersHandle,
    BUFFER_HANDLE *toBeUsedRequestContent, bool *isOriginalRequestContent,
    unsigned int** toBeUsedStatusCode,
    HTTP_HEADERS_HANDLE *toBeUsedResponseHttpHeadersHandle, bool *isOriginalResponseHttpHeadersHandle,
    BUFFER_HANDLE *toBeUsedResponseContent, bool *isOriginalResponseContent)
{
    int result;
    (void)requestType;
    /*Codes_SRS_HTTPAPIEX_02_013: [If requestContent is NULL then HTTPAPIEX_ExecuteRequest shall behave as if a buffer of zero size would have been used, that is, it shall call HTTPAPI_ExecuteRequest with parameter content = NULL and contentLength = 0.]*/
    /*Codes_SRS_HTTPAPIEX_02_014: [If requestContent is not NULL then its content and its size shall be used for parameters content and contentLength of HTTPAPI_ExecuteRequest.] */
    if (buildBufferIfNotExist(requestContent, isOriginalRequestContent, toBeUsedRequestContent) != 0)
    {
        result = __LINE__;
        LogError("unable to build the request content");
    }
    else
    {
        if (buildRequestHttpHeadersHandle(handle, *toBeUsedRequestContent, requestHttpHeadersHandle, isOriginalRequestHttpHeadersHandle, toBeUsedRequestHttpHeadersHandle) != 0)
        {
            /*Codes_SRS_HTTPAPIEX_02_010: [If any of the operations in SRS_HTTAPIEX_02_009 fails, then HTTPAPIEX_ExecuteRequest shall return HTTPAPIEX_ERROR.] */
            result = __LINE__;
            if (*isOriginalRequestContent == false) 
            {
                BUFFER_delete(*toBeUsedRequestContent);
            }
            LogError("unable to build the request http headers handle");
        }
        else
        {
            /*Codes_SRS_HTTPAPIEX_02_008: [If parameter relativePath is NULL then HTTPAPIEX_INVALID_ARG shall not assume a relative path - that is, it will assume an empty path ("").] */
            if (relativePath == NULL)
            {
                *toBeUsedRelativePath = "";
            }
            else
            {
                *toBeUsedRelativePath = relativePath;
            }

            /*Codes_SRS_HTTPAPIEX_02_015: [If statusCode is NULL then HTTPAPIEX_ExecuteRequest shall not write in statusCode the HTTP status code, and it will use a temporary internal int for parameter statusCode to the calls of HTTPAPI_ExecuteRequest.] */
            if (statusCode == NULL)
            {
                /*Codes_SRS_HTTPAPIEX_02_016: [If statusCode is not NULL then If statusCode is NULL then HTTPAPIEX_ExecuteRequest shall use it for parameter statusCode to the calls of HTTPAPI_ExecuteRequest.] */
                *toBeUsedStatusCode = &dummyStatusCode;
            }
            else
            {
                *toBeUsedStatusCode = statusCode;
            }

            /*Codes_SRS_HTTPAPIEX_02_017: [If responseHeaders handle is NULL then HTTPAPIEX_ExecuteRequest shall create a temporary internal instance of HTTPHEADERS object and use that for responseHeaders parameter of HTTPAPI_ExecuteRequest call.] */
            /*Codes_SRS_HTTPAPIEX_02_019: [If responseHeaders is not NULL, then then HTTPAPIEX_ExecuteRequest shall use that object as parameter responseHeaders of HTTPAPI_ExecuteRequest call.] */
            if (buildResponseHttpHeadersHandle(responseHttpHeadersHandle, isOriginalResponseHttpHeadersHandle, toBeUsedResponseHttpHeadersHandle) != 0)
            {
                /*Codes_SRS_HTTPAPIEX_02_018: [If creating the temporary http headers in SRS_HTTPAPIEX_02_017 fails then HTTPAPIEX_ExecuteRequest shall return HTTPAPIEX_ERROR.] */
                result = __LINE__;
                if (*isOriginalRequestContent == false)
                {
                    BUFFER_delete(*toBeUsedRequestContent);
                }
                if (*isOriginalRequestHttpHeadersHandle == false)
                {
                    HTTPHeaders_Free(*toBeUsedRequestHttpHeadersHandle);
                }
                LogError("unable to build response content");
            }
            else
            {
                /*Codes_SRS_HTTPAPIEX_02_020: [If responseContent is NULL then HTTPAPIEX_ExecuteRequest shall create a temporary internal BUFFER object and use that as parameter responseContent of HTTPAPI_ExecuteRequest call.] */
                /*Codes_SRS_HTTPAPIEX_02_022: [If responseContent is not NULL then HTTPAPIEX_ExecuteRequest use that as parameter responseContent of HTTPAPI_ExecuteRequest call.] */
                if (buildBufferIfNotExist(responseContent, isOriginalResponseContent, toBeUsedResponseContent) != 0)
                {
                    /*Codes_SRS_HTTPAPIEX_02_021: [If creating the BUFFER_HANDLE in SRS_HTTPAPIEX_02_020 fails, then HTTPAPIEX_ExecuteRequest shall return HTTPAPIEX_ERROR.] */
                    result = __LINE__;
                    if (*isOriginalRequestContent == false)
                    {
                        BUFFER_delete(*toBeUsedRequestContent);
                    }
                    if (*isOriginalRequestHttpHeadersHandle == false)
                    {
                        HTTPHeaders_Free(*toBeUsedRequestHttpHeadersHandle);
                    }
                    if (*isOriginalResponseHttpHeadersHandle == false)
                    {
                        HTTPHeaders_Free(*toBeUsedResponseHttpHeadersHandle);
                    }
                    LogError("unable to build response content");
                }
                else
                {
                    result = 0;
                }
            }
        }
    }
    return result;
}

HTTPAPIEX_RESULT HTTPAPIEX_ExecuteRequest(HTTPAPIEX_HANDLE handle, HTTPAPI_REQUEST_TYPE requestType, const char* relativePath,
    HTTP_HEADERS_HANDLE requestHttpHeadersHandle, BUFFER_HANDLE requestContent, unsigned int* statusCode,
    HTTP_HEADERS_HANDLE responseHttpHeadersHandle, BUFFER_HANDLE responseContent)
{
    HTTPAPIEX_RESULT result;
    /*Codes_SRS_HTTPAPIEX_02_006: [If parameter handle is NULL then HTTPAPIEX_ExecuteRequest shall fail and return HTTPAPIEX_INVALID_ARG.]*/
    if (handle == NULL)
    {
        result = HTTPAPIEX_INVALID_ARG;
        LOG_HTTAPIEX_ERROR();
    }
    else
    {
        /*Codes_SRS_HTTPAPIEX_02_007: [If parameter requestType does not indicate a valid request, HTTPAPIEX_ExecuteRequest shall fail and return HTTPAPIEX_INVALID_ARG.] */
        if (requestType >= COUNT_ARG(HTTPAPI_REQUEST_TYPE_VALUES))
        {
            result = HTTPAPIEX_INVALID_ARG;
            LOG_HTTAPIEX_ERROR();
        }
        else
        {
            HTTPAPIEX_HANDLE_DATA *handleData = (HTTPAPIEX_HANDLE_DATA *)handle;

            /*call to buildAll*/
            const char* toBeUsedRelativePath;
            HTTP_HEADERS_HANDLE toBeUsedRequestHttpHeadersHandle; bool isOriginalRequestHttpHeadersHandle;
            BUFFER_HANDLE toBeUsedRequestContent; bool isOriginalRequestContent;
            unsigned int* toBeUsedStatusCode;
            HTTP_HEADERS_HANDLE toBeUsedResponseHttpHeadersHandle; bool isOriginalResponseHttpHeadersHandle;
            BUFFER_HANDLE toBeUsedResponseContent;  bool isOriginalResponseContent;

            if (buildAllRequests(handleData, requestType, relativePath, requestHttpHeadersHandle, requestContent, statusCode, responseHttpHeadersHandle, responseContent,
                &toBeUsedRelativePath,
                &toBeUsedRequestHttpHeadersHandle, &isOriginalRequestHttpHeadersHandle,
                &toBeUsedRequestContent, &isOriginalRequestContent,
                &toBeUsedStatusCode,
                &toBeUsedResponseHttpHeadersHandle, &isOriginalResponseHttpHeadersHandle,
                &toBeUsedResponseContent, &isOriginalResponseContent) != 0)
            {
                result = HTTPAPIEX_ERROR;
                LOG_HTTAPIEX_ERROR();
            }
            else
            {

                /*Codes_SRS_HTTPAPIEX_02_023: [HTTPAPIEX_ExecuteRequest shall try to execute the HTTP call by ensuring the following API call sequence is respected:]*/
                /*Codes_SRS_HTTPAPIEX_02_024: [If any point in the sequence fails, HTTPAPIEX_ExecuteRequest shall attempt to recover by going back to the previous step and retrying that step.]*/
                /*Codes_SRS_HTTPAPIEX_02_025: [If the first step fails, then the sequence fails.]*/
                /*Codes_SRS_HTTPAPIEX_02_026: [A step shall be retried at most once.]*/
                /*Codes_SRS_HTTPAPIEX_02_027: [If a step has been retried then all subsequent steps shall be retried too.]*/
                bool st[3] = { false, false, false }; /*the three levels of possible failure in resilient send: HTTAPI_Init, HTTPAPI_CreateConnection, HTTPAPI_ExecuteRequest*/
                if (handleData->k == -1)
                {
                    handleData->k = 0;
                }

                do
                {
                    bool goOn;

                    if (handleData->k > 2)
                    {
                        /* error */
                        break;
                    }

                    if (st[handleData->k] == true) /*already been tried*/
                    {
                        goOn = false;
                    }
                    else
                    {
                        switch (handleData->k)
                        {
                        case 0:
                        {
                            if (HTTPAPI_Init() != HTTPAPI_OK)
                            {
                                goOn = false;
                            }
                            else
                            {
                                goOn = true;
                            }
                            break;
                        }
                        case 1:
                        {
                            if ((handleData->httpHandle = HTTPAPI_CreateConnection(STRING_c_str(handleData->hostName))) == NULL)
                            {
                                goOn = false;
                            }
                            else
                            {
                                size_t i;
                                size_t vectorSize = VECTOR_size(handleData->savedOptions);
                                for (i = 0; i < vectorSize; i++)
                                {
                                    /*Codes_SRS_HTTPAPIEX_02_035: [HTTPAPIEX_ExecuteRequest shall pass all the saved options (see HTTPAPIEX_SetOption) to the newly create HTTPAPI_HANDLE in step 2 by calling HTTPAPI_SetOption.]*/
                                    /*Codes_SRS_HTTPAPIEX_02_036: [If setting the option fails, then the failure shall be ignored.] */
                                    HTTPAPIEX_SAVED_OPTION* option = VECTOR_element(handleData->savedOptions, i);
                                    if (HTTPAPI_SetOption(handleData->httpHandle, option->optionName, option->value) != HTTPAPI_OK)
                                    {
                                        LogError("HTTPAPI_SetOption failed when called for option %s", option->optionName);
                                    }
                                }
                                goOn = true;
                            }
                            break;
                        }
                        case 2:
                        {
                            if (HTTPAPI_ExecuteRequest(handleData->httpHandle, requestType, toBeUsedRelativePath, toBeUsedRequestHttpHeadersHandle, BUFFER_u_char(toBeUsedRequestContent), BUFFER_length(toBeUsedRequestContent), toBeUsedStatusCode, toBeUsedResponseHttpHeadersHandle, toBeUsedResponseContent) != HTTPAPI_OK)
                            {
                                goOn = false;
                            }
                            else
                            {
                                goOn = true;
                            }
                            break;
                        }
                        default:
                        {
                            /*serious error*/
                            goOn = false;
                            break;
                        }
                        }
                    }

                    if (goOn)
                    {
                        if (handleData->k == 2)
                        {
                            /*Codes_SRS_HTTPAPIEX_02_028: [HTTPAPIEX_ExecuteRequest shall return HTTPAPIEX_OK when a call to HTTPAPI_ExecuteRequest has been completed successfully.]*/
                            result = HTTPAPIEX_OK;
                            goto out;
                        }
                        else
                        {
                            st[handleData->k] = true;
                            handleData->k++;
                            st[handleData->k] = false;
                        }
                    }
                    else
                    {
                        st[handleData->k] = false;
                        handleData->k--;
                        switch (handleData->k)
                        {
                        case 0:
                        {
                            HTTPAPI_Deinit();
                            break;
                        }
                        case 1:
                        {
                            HTTPAPI_CloseConnection(handleData->httpHandle);
                            handleData->httpHandle = NULL;
                            break;
                        }
                        case 2:
                        {
                            break;
                        }
                        default:
                        {
                            break;
                        }
                        }
                    }
                } while (handleData->k >= 0);
                /*Codes_SRS_HTTPAPIEX_02_029: [Otherwise, HTTAPIEX_ExecuteRequest shall return HTTPAPIEX_RECOVERYFAILED.] */
                result = HTTPAPIEX_RECOVERYFAILED;
                LogError("unable to recover sending to a working state");
            out:;
                /*in all cases, unbuild the temporaries*/
                if (isOriginalRequestContent == false)
                {
                    BUFFER_delete(toBeUsedRequestContent);
                }
                if (isOriginalRequestHttpHeadersHandle == false)
                {
                    HTTPHeaders_Free(toBeUsedRequestHttpHeadersHandle);
                }
                if (isOriginalResponseContent == false)
                {
                    BUFFER_delete(toBeUsedResponseContent);
                }
                if (isOriginalResponseHttpHeadersHandle == false)
                {
                    HTTPHeaders_Free(toBeUsedResponseHttpHeadersHandle);
                }
            }
        }
    }
    return result;
}


void HTTPAPIEX_Destroy(HTTPAPIEX_HANDLE handle)
{
    if (handle != NULL)
    {
        /*Codes_SRS_HTTPAPIEX_02_042: [HTTPAPIEX_Destroy shall free all the resources used by HTTAPIEX_HANDLE.]*/
        size_t i;
        size_t vectorSize;
        HTTPAPIEX_HANDLE_DATA* handleData = (HTTPAPIEX_HANDLE_DATA*)handle;
        
        if (handleData->k == 2)
        {
            HTTPAPI_CloseConnection(handleData->httpHandle);
            HTTPAPI_Deinit();
        }
        STRING_delete(handleData->hostName);

        vectorSize = VECTOR_size(handleData->savedOptions);
        for (i = 0; i < vectorSize; i++)
        {
            HTTPAPIEX_SAVED_OPTION*savedOption = VECTOR_element(handleData->savedOptions, i);
            free((void*)savedOption->optionName);
            free((void*)savedOption->value);
        }
        VECTOR_destroy(handleData->savedOptions);

        free(handle);
    }
    else
    {
        /*Codes_SRS_HTTPAPIEX_02_043: [If parameter handle is NULL then HTTPAPIEX_Destroy shall take no action.] */
    }
}

static bool sameName(const void* element, const void* value)
{
    return (strcmp(((HTTPAPIEX_SAVED_OPTION*)element)->optionName, value) == 0) ? true : false;
}

/*return 0 on success, any other value is error*/
/*obs: value is already cloned at the time of calling this function */
static int createOrUpdateOption(HTTPAPIEX_HANDLE_DATA* handleData, const char* optionName, const void* value)
{
    /*this function is called after the option value has been saved (cloned)*/
    int result;
    
    /*decide bwtween update or create*/
    HTTPAPIEX_SAVED_OPTION* whereIsIt = VECTOR_find_if(handleData->savedOptions, sameName, optionName);
    if (whereIsIt != NULL)
    {
        free((void*)(whereIsIt->value));
        whereIsIt->value = value;
        result = 0;
    }
    else
    {
        HTTPAPIEX_SAVED_OPTION newOption;
        if (mallocAndStrcpy_s((char**)&(newOption.optionName), optionName) != 0)
        {
            free((void*)value);
            result = __LINE__;
        }
        else
        {
            newOption.value = value;
            if (VECTOR_push_back(handleData->savedOptions, &newOption, 1) != 0)
            {
                LogError("unable to VECTOR_push_back");
                free((void*)newOption.optionName);
                free((void*)value);
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

HTTPAPIEX_RESULT HTTPAPIEX_SetOption(HTTPAPIEX_HANDLE handle, const char* optionName, const void* value)
{
    HTTPAPIEX_RESULT result;
    /*Codes_SRS_HTTPAPIEX_02_032: [If parameter handle is NULL then HTTPAPIEX_SetOption shall return HTTPAPIEX_INVALID_ARG.] */
    /*Codes_SRS_HTTPAPIEX_02_033: [If parameter optionName is NULL then HTTPAPIEX_SetOption shall return HTTPAPIEX_INVALID_ARG.] */
    /*Codes_SRS_HTTPAPIEX_02_034: [If parameter value is NULL then HTTPAPIEX_SetOption shall return HTTPAPIEX_INVALID_ARG.] */
    if (
        (handle == NULL) ||
        (optionName == NULL) ||
        (value == NULL)
        )
    {
        result = HTTPAPIEX_INVALID_ARG;
        LOG_HTTAPIEX_ERROR();
    }
    else
    {
        const void* savedOption;
        HTTPAPI_RESULT saveOptionResult;

        /*Codes_SRS_HTTPAPIEX_02_037: [HTTPAPIEX_SetOption shall attempt to save the value of the option by calling HTTPAPI_CloneOption passing optionName and value, irrespective of the existence of a HTTPAPI_HANDLE] */
        saveOptionResult = HTTPAPI_CloneOption(optionName, value, &savedOption);

        if(saveOptionResult == HTTPAPI_INVALID_ARG)
        {
            /*Codes_SRS_HTTPAPIEX_02_038: [If HTTPAPI_CloneOption returns HTTPAPI_INVALID_ARG then HTTPAPIEX shall return HTTPAPIEX_INVALID_ARG.] */
            result = HTTPAPIEX_INVALID_ARG;
            LOG_HTTAPIEX_ERROR();
        }
        else if (saveOptionResult != HTTPAPI_OK)
        {
            /*Codes_SRS_HTTPAPIEX_02_040: [For all other return values of HTTPAPI_SetOption, HTTPIAPIEX_SetOption shall return HTTPAPIEX_ERROR.] */
            result = HTTPAPIEX_ERROR;
            LOG_HTTAPIEX_ERROR();
        }
        else
        {
            HTTPAPIEX_HANDLE_DATA* handleData = (HTTPAPIEX_HANDLE_DATA*)handle;
            /*Codes_SRS_HTTPAPIEX_02_039: [If HTTPAPI_CloneOption returns HTTPAPI_OK then HTTPAPIEX_SetOption shall create or update the pair optionName/value.]*/
            if (createOrUpdateOption(handleData, optionName, savedOption) != 0)
            {
                /*Codes_SRS_HTTPAPIEX_02_041: [If creating or updating the pair optionName/value fails then shall return HTTPAPIEX_ERROR.] */
                result = HTTPAPIEX_ERROR;
                LOG_HTTAPIEX_ERROR();
                
            }
            else
            {
                /*Codes_SRS_HTTPAPIEX_02_031: [If HTTPAPI_HANDLE exists then HTTPAPIEX_SetOption shall call HTTPAPI_SetOption passing the same optionName and value and shall return a value conforming to the below table:] */
                if (handleData->httpHandle != NULL)
                {
                    HTTPAPI_RESULT HTTPAPI_result = HTTPAPI_SetOption(handleData->httpHandle, optionName, value);
                    if (HTTPAPI_result == HTTPAPI_OK)
                    {
                        result = HTTPAPIEX_OK;
                    }
                    else if (HTTPAPI_result == HTTPAPI_INVALID_ARG)
                    {
                        result = HTTPAPIEX_INVALID_ARG;
                        LOG_HTTAPIEX_ERROR();
                    }
                    else
                    {
                        result = HTTPAPIEX_ERROR;
                        LOG_HTTAPIEX_ERROR();
                    }
                }
                else
                {
                    result = HTTPAPIEX_OK;
                }
            }
        }
    }
    return result;
}
