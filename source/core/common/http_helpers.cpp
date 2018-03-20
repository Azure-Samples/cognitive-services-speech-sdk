//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// http_helpers.cpp: Implementation definitions for *Http* helper methods
//

#include "stdafx.h"
#include "http_helpers.h"
#include <string.h>
#include "azure_c_shared_utility_buffer_wrapper.h"
#include "azure_c_shared_utility_httpapi_wrapper.h"
#include "azure_c_shared_utility_httpapiex_wrapper.h"


namespace CARBON_IMPL_NAMESPACE() {


std::string SpxHttpDownloadString(const char* httpHostName, const char* httpRelativePath, unsigned int* pstatusCode, std::map<std::string, std::string>* presponseHeaders)
{
    // TODO: RobCh: Consider: Should we refactor this to speed up calls?
    //
    //      We could initialize the HTTPApi once, and reuse that ... 
    //      Similarly we could cache the connection to httpHostName ...
    //      We could make this into a service available to sites via CSpxResourceManager ... 

    SPX_DBG_TRACE_FUNCTION();
    SPX_INIT_HR(hr);

    std::string downloadedString;
    unsigned int statusCode = 0;

    // Initialize all "http" related HANDLEs to NULL, so function clean-up will do the right thing...
    HTTPAPI_RESULT httpInit = HTTPAPI_OK;
    HTTPAPI_RESULT executeResult = HTTPAPI_OK;
    HTTP_HANDLE httpHandle = NULL;
    HTTP_HEADERS_RESULT headerResult = HTTP_HEADERS_OK;
    HTTP_HEADERS_HANDLE requestHttpHeaders = NULL;
    HTTP_HEADERS_HANDLE responseHttpHeaders = NULL;
    BUFFER_HANDLE responseBuffer = NULL;

    // Initialize the HTTP API module... 
    httpInit = HTTPAPI_Init();
    SPX_IFTRUE_EXITFN_WHR(HTTPAPI_OK != httpInit, hr = SPXERR_ABORT);

    // Prepare the connection to the host
    httpHandle = HTTPAPI_CreateConnection(httpHostName);
    SPX_IFTRUE_EXITFN_WHR(httpHandle == NULL, hr = SPXERR_ABORT);

    // Allocate and initialize the HTTP request headers: Host and Content-Length
    requestHttpHeaders = HTTPHeaders_Alloc();
    SPX_IFTRUE_EXITFN_WHR(requestHttpHeaders == NULL, hr = SPXERR_ABORT);

    headerResult = HTTPHeaders_ReplaceHeaderNameValuePair(requestHttpHeaders, "Host", httpHostName);
    SPX_IFTRUE_EXITFN_WHR(headerResult != HTTP_HEADERS_OK, hr = SPXERR_ABORT);

    headerResult = HTTPHeaders_ReplaceHeaderNameValuePair(requestHttpHeaders, "Content-Length", "0");
    SPX_IFTRUE_EXITFN_WHR(headerResult != HTTP_HEADERS_OK, hr = SPXERR_ABORT);

    // Create a BUFFER to hold the response from the service
    responseBuffer = BUFFER_new();
    SPX_IFTRUE_EXITFN_WHR(responseBuffer == NULL, hr = SPXERR_ABORT);

    // Allocate space to recieve the response headers if necessary
    responseHttpHeaders = presponseHeaders != nullptr ? HTTPHeaders_Alloc() : NULL;
    SPX_IFTRUE_EXITFN_WHR(presponseHeaders != nullptr && responseHttpHeaders == NULL, hr = SPXERR_ABORT);

    // Execute the request
    executeResult = HTTPAPI_ExecuteRequest(
        httpHandle,
        HTTPAPI_REQUEST_GET,
        httpRelativePath,
        requestHttpHeaders,
        NULL,
        0,
        &statusCode,
        responseHttpHeaders,
        responseBuffer);
    SPX_IFTRUE_EXITFN_WHR(executeResult != HTTPAPI_OK, hr = SPXERR_ABORT);

    // return the status code if the caller asked for it ... 
    if (pstatusCode != nullptr)
    {
        *pstatusCode = statusCode;
    }

    // return the response headers if the caller asked for them... 
    if (presponseHeaders != nullptr)
    {
        // how many headers do we have? 
        size_t headerCount = 0;
        headerResult = HTTPHeaders_GetHeaderCount(responseHttpHeaders, &headerCount);

        // loop thru each header name/value 
        for (size_t i = 0; i < headerCount; i++)
        {
            // get the header (e.g. in "NAME: VALUE" format)... 
            char* rawHeader = nullptr;
            headerResult = HTTPHeaders_GetHeader(responseHttpHeaders, i, &rawHeader);

            // find the "NAME" part (from the beginning, up to, and not including, the trailing ':')
            auto nameBeginPtr = rawHeader;
            auto nameEndPtr = strchr(nameBeginPtr, ':');
            SPX_IFTRUE_EXITFN_WHR(nameEndPtr == nullptr || nameEndPtr[0] != ':', hr = SPXERR_ABORT);
            std::string headerName(nameBeginPtr, nameEndPtr);

            // find the "VALUE" part (by skipping the ':' and leading spaces before the value)
            auto valueBeginPtr = nameEndPtr + 1;
            while (isspace(*valueBeginPtr))
            {
                valueBeginPtr++;
            }
            std::string headerValue(valueBeginPtr);

            // stick the header name/value into the map
            SPX_DBG_TRACE_VERBOSE("Inserting header '%s': '%s'", headerName.c_str(), headerValue.c_str());
            presponseHeaders->emplace(std::move(headerName), std::move(headerValue));

            // clean up the raw header
            free(rawHeader);
            rawHeader = nullptr;
        }
    }

    // if our status code was 200, we're good to go ... let's return the downloaded string
    if (statusCode == 200)
    {
        const size_t countResponseChars = BUFFER_length(responseBuffer);

        const unsigned char* responseChars = NULL;
        BUFFER_content(responseBuffer, &responseChars);

        if (countResponseChars > 0 && responseChars != nullptr)
        {
            downloadedString = std::string((char*)responseChars, countResponseChars);
        }
    }

    SPX_EXITFN_CLEANUP:

    // Free the response BUFFER
    if (responseBuffer != NULL)
    {
        BUFFER_delete(responseBuffer);
        responseBuffer = NULL;
    }

    // Free the response headers
    if (responseHttpHeaders != NULL)
    {
        HTTPHeaders_Free(responseHttpHeaders);
        responseHttpHeaders = NULL;
    }

    // Free the request headers
    if (requestHttpHeaders != NULL)
    {
        HTTPHeaders_Free(requestHttpHeaders);
        requestHttpHeaders = NULL;
    }

    // Free the connection to the service...
    if (httpHandle != NULL)
    {
        HTTPAPI_CloseConnection(httpHandle);
        httpHandle = NULL;
    }

    // Uninitialize the HTTPAPI module...
    HTTPAPI_Deinit();

    // If we've been successful thus far, we're good ... If not, throw the error
    SPX_THROW_ON_FAIL(hr);

    return downloadedString;
}


} // CARBON_IMPL_NAMESPACE
