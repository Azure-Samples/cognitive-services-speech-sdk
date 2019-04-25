//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// authenticator.cpp: Implementation definitions for CSpxRestTtsAuthenticator C++ class
//

#include "stdafx.h"
#include "httpapi.h"
#include "rest_tts_helper.h"
#include "authenticator.h"
#include "azure_c_shared_utility/shared_util_options.h"

#define SPX_DBG_TRACE_REST_TTS_AUTHENTICATOR 0


namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {


CSpxRestTtsAuthenticator::CSpxRestTtsAuthenticator(const std::string& issueTokenUri, const std::string& subscriptionKey) :
    m_issueTokenUri(issueTokenUri), m_subscriptionKey(subscriptionKey)
{
    SPX_DBG_TRACE_VERBOSE_IF(SPX_DBG_TRACE_REST_TTS_AUTHENTICATOR, __FUNCTION__);
    Init();
}

CSpxRestTtsAuthenticator::~CSpxRestTtsAuthenticator()
{
    SPX_DBG_TRACE_VERBOSE_IF(SPX_DBG_TRACE_REST_TTS_AUTHENTICATOR, __FUNCTION__);
    Term();
}

void CSpxRestTtsAuthenticator::Init()
{
    // Access token expires every 10 minutes. Renew it every 9 minutes only
    m_accessTokenRenewer.Start(9 * 60 * 1000, std::bind([this]() {
        RenewAccessToken();
        m_accessTokenInitialized = true;
    }));

    while (!m_accessTokenInitialized)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void CSpxRestTtsAuthenticator::Term()
{
    m_accessTokenRenewer.Expire(); // Stop the timer
}

std::string CSpxRestTtsAuthenticator::GetAccessToken()
{
    SPX_DBG_TRACE_VERBOSE_IF(SPX_DBG_TRACE_REST_TTS_AUTHENTICATOR, __FUNCTION__);

    std::unique_lock<std::mutex> lock(m_mutex);
    return m_accessToken;
}

void CSpxRestTtsAuthenticator::RenewAccessToken()
{
    SPX_DBG_TRACE_VERBOSE_IF(SPX_DBG_TRACE_REST_TTS_AUTHENTICATOR, __FUNCTION__);

    std::unique_lock<std::mutex> lock(m_mutex);
    m_accessToken = HttpPost(m_issueTokenUri, m_subscriptionKey);
}

std::string CSpxRestTtsAuthenticator::HttpPost(const std::string& issueTokenUri, const std::string& subscriptionKey)
{
    SPX_DBG_TRACE_VERBOSE_IF(SPX_DBG_TRACE_REST_TTS_AUTHENTICATOR, __FUNCTION__);

    // Parse host name & path from URL
    bool secure = false;
    const char* host = nullptr;
    size_t host_length = 0;
    int port = 0;
    const char* path = nullptr;
    size_t path_length = 0;
    const char* query = nullptr;
    size_t query_length = 0;
    CSpxRestTtsHelper::ParseHttpUrl(issueTokenUri.data(), &secure, &host, &host_length, &port, &path, &path_length, &query, &query_length);

    std::string host_str = std::string(host, host_length);
    std::string path_str = std::string("/") + std::string(path, path_length);
    std::string query_str = std::string("?") + std::string(query, query_length);

    // Allocate resources
    HTTP_HANDLE http_connect = HTTPAPI_CreateConnection(host_str.data());
    if (!http_connect)
    {
        throw std::runtime_error("Could not create HTTP connection");
    }

#ifdef SPEECHSDK_USE_OPENSSL
    int tls_version = OPTION_TLS_VERSION_1_2;
    if (HTTPAPI_SetOption(http_connect, OPTION_TLS_VERSION, &tls_version) != HTTPAPI_OK)
    {
        HTTPAPI_CloseConnection(http_connect);
        throw std::runtime_error("Could not set TLS 1.2 option");
    }
#endif

    HTTP_HEADERS_HANDLE httpRequestHeaders = HTTPHeaders_Alloc();
    if (!httpRequestHeaders)
    {
        HTTPAPI_CloseConnection(http_connect);
        throw std::runtime_error("Could not allocate HTTP request headers");
    }

    HTTP_HEADERS_HANDLE httpResponseHeaders = HTTPHeaders_Alloc();
    if (!httpResponseHeaders)
    {
        HTTPHeaders_Free(httpRequestHeaders);
        HTTPAPI_CloseConnection(http_connect);
        throw std::runtime_error("Could not allocate HTTP response headers");
    }

    BUFFER_HANDLE buffer = BUFFER_new();
    if (!buffer)
    {
        HTTPHeaders_Free(httpRequestHeaders);
        HTTPHeaders_Free(httpResponseHeaders);
        HTTPAPI_CloseConnection(http_connect);
        throw std::runtime_error("Could not allocate HTTP data buffer");
    }

    std::string accessToken;
    try
    {
        // Add http headers
        if (HTTPHeaders_AddHeaderNameValuePair(httpRequestHeaders, "Host", host_str.data()) != HTTP_HEADERS_OK)
        {
            throw std::runtime_error("Could not add HTTP request header: Host");
        }

        if (HTTPHeaders_AddHeaderNameValuePair(httpRequestHeaders, "Ocp-Apim-Subscription-Key", subscriptionKey.data()) != HTTP_HEADERS_OK)
        {
            throw std::runtime_error("Could not add HTTP request header: Ocp-Apim-Subscription-Key");
        }

        if (HTTPHeaders_AddHeaderNameValuePair(httpRequestHeaders, "Content-Length", "0") != HTTP_HEADERS_OK)
        {
            throw std::runtime_error("Could not add HTTP request header: Content-Length");
        }

        // Execute http request
        unsigned int statusCode = 0;
        HTTPAPI_RESULT result = HTTPAPI_ExecuteRequest(
            http_connect,
            HTTPAPI_REQUEST_POST,
            (path_str + query_str).data(),
            httpRequestHeaders,
            nullptr,
            0,
            &statusCode,
            httpResponseHeaders,
            buffer,
            nullptr,
            nullptr);

        // Check result
        if (result != HTTPAPI_OK || statusCode < 200 || statusCode >= 300)
        {
            // Build error message
            std::stringstream errorMessage;
            errorMessage << "Failed to execute http request (azure-c-shared) for cognitive service authentication. ";
            errorMessage << "HTTPAPI result code = " << ENUM_TO_STRING(HTTPAPI_RESULT, result) << ".";
            if (result == HTTPAPI_OK)
            {
                errorMessage << " HTTP status code = " << statusCode << ".";
            }

            SPX_TRACE_ERROR(errorMessage.str().data());
        }
        else
        {
            unsigned char* buffer_content = BUFFER_u_char(buffer);
            size_t buffer_length = BUFFER_length(buffer);
            accessToken = std::string((const char *)buffer_content, buffer_length);
        }
    }
    catch (...)
    {
        // Release resources
        BUFFER_delete(buffer);
        HTTPHeaders_Free(httpRequestHeaders);
        HTTPHeaders_Free(httpResponseHeaders);
        HTTPAPI_CloseConnection(http_connect);

        throw;
    }

    // Release resources
    BUFFER_delete(buffer);
    HTTPHeaders_Free(httpRequestHeaders);
    HTTPHeaders_Free(httpResponseHeaders);
    HTTPAPI_CloseConnection(http_connect);

    return accessToken;
}


} } } } // Microsoft::CognitiveServices::Speech::Impl
