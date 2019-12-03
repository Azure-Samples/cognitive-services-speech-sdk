//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// authenticator.cpp: Implementation definitions for CSpxRestTtsAuthenticator C++ class
//

#include "stdafx.h"
#include "synthesis_helper.h"
#include "authenticator.h"
#include "azure_c_shared_utility_httpapi_wrapper.h"
#include "azure_c_shared_utility/shared_util_options.h"

#define SPX_DBG_TRACE_REST_TTS_AUTHENTICATOR 0


namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {


CSpxRestTtsAuthenticator::CSpxRestTtsAuthenticator(const std::string& issueTokenUri, const std::string& subscriptionKey,
    const std::string& proxyHost, int proxyPort, const std::string& proxyUsername, const std::string& proxyPassword) :
    m_issueTokenUri(issueTokenUri), m_subscriptionKey(subscriptionKey),
    m_proxyHost(proxyHost), m_proxyPort(proxyPort), m_proxyUsername(proxyUsername), m_proxyPassword(proxyPassword)
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
    if (m_subscriptionKey.empty())
    {
        // Don't start the thread if subscription key is not provided
        return;
    }

    // Access token expires every 10 minutes. Renew it every 9 minutes only
    m_accessTokenRenewer.Start(9 * 60 * 1000, std::bind([this]() {
        RenewAccessToken();
    }));
}

void CSpxRestTtsAuthenticator::Term()
{
    m_accessTokenRenewer.Expire(); // Stop the timer
}

std::string CSpxRestTtsAuthenticator::GetAccessToken()
{
    SPX_DBG_TRACE_VERBOSE_IF(SPX_DBG_TRACE_REST_TTS_AUTHENTICATOR, __FUNCTION__);

    if (m_subscriptionKey.empty())
    {
        return std::string();
    }

    // Access token expires every 10 minutes. Check if it refreshed within 9.5 minutes.
    m_accessTokenRenewer.WaitUntilValid(570000);  // 9.5 * 60 * 1000

    return m_accessToken;
}

void CSpxRestTtsAuthenticator::RenewAccessToken()
{
    SPX_DBG_TRACE_VERBOSE_IF(SPX_DBG_TRACE_REST_TTS_AUTHENTICATOR, __FUNCTION__);

    m_accessToken = HttpPost(m_issueTokenUri, m_subscriptionKey, m_proxyHost, m_proxyPort, m_proxyUsername, m_proxyPassword);
}

std::string CSpxRestTtsAuthenticator::HttpPost(const std::string& issueTokenUri, const std::string& subscriptionKey,
    const std::string& proxyHost, int proxyPort, const std::string& proxyUsername, const std::string& proxyPassword)
{
    SPX_DBG_TRACE_VERBOSE_IF(SPX_DBG_TRACE_REST_TTS_AUTHENTICATOR, __FUNCTION__);

    // Parse URL
    auto url = HttpUtils::ParseUrl(issueTokenUri);

    // Allocate resources
    HTTP_HANDLE http_connect = HTTPAPI_CreateConnection_Advanced(
        url.host.data(), url.port, url.secure, proxyHost.data(), proxyPort, proxyUsername.data(), proxyPassword.data());
    if (!http_connect)
    {
        SPX_TRACE_ERROR("Could not create HTTP connection");
        return std::string();
    }

#ifdef SPEECHSDK_USE_OPENSSL
    int tls_version = OPTION_TLS_VERSION_1_2;
    if (HTTPAPI_SetOption(http_connect, OPTION_TLS_VERSION, &tls_version) != HTTPAPI_OK)
    {
        HTTPAPI_CloseConnection(http_connect);
        SPX_TRACE_ERROR("Could not set TLS 1.2 option");
        return std::string();
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
        if (HTTPHeaders_AddHeaderNameValuePair(httpRequestHeaders, "Host", url.host.data()) != HTTP_HEADERS_OK)
        {
            throw std::runtime_error("Could not add HTTP request header: Host");
        }

        if (HTTPHeaders_AddHeaderNameValuePair(httpRequestHeaders, SUBSCRIPTION_KEY_NAME, subscriptionKey.data()) != HTTP_HEADERS_OK)
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
            (std::string("/") + url.path + std::string("?") + url.query).data(),
            httpRequestHeaders,
            nullptr,
            0,
            &statusCode,
            httpResponseHeaders,
            buffer);

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

            SPX_TRACE_ERROR("%s", errorMessage.str().data());
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
