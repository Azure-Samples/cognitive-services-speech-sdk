//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#include "stdafx.h"
#include <sstream>
#include <azure_c_shared_utility_urlencode_wrapper.h>
#include "http_utils.h"
#include "http_request.h"
#include "http_response.h"

#ifdef SPEECHSDK_USE_OPENSSL
    #include <azure_c_shared_utility/shared_util_options.h>
#endif

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

    HttpRequest::HttpRequest(const std::string& host, int port, bool isSecure) :
        m_endpoint(),
        m_handle(),
        m_requestHeaders()
    {
        m_endpoint
            .Scheme(isSecure ? UriScheme::HTTPS : UriScheme::HTTP)
            .Host(host)
            .Port(port);

        m_requestHeaders = HTTPHeaders_Alloc();
        if (m_requestHeaders == nullptr)
        {
            throw std::bad_alloc();
        }
        
        SetRequestHeader("Host", m_endpoint.Host());
    }

    HttpRequest::HttpRequest(const HttpEndpointInfo& endpoint) :
        m_endpoint(endpoint),
        m_handle(),
        m_requestHeaders()
    {
        if (!endpoint.IsValid())
        {
            throw std::invalid_argument("You must specify valid HTTP endpoint information");
        }

        m_requestHeaders = HTTPHeaders_Alloc();
        if (m_requestHeaders == nullptr)
        {
            throw std::bad_alloc();
        }

        SetRequestHeader("Host", m_endpoint.Host());
    }

    HttpRequest::~HttpRequest()
    {
        if (m_handle)
        {
            HTTPAPI_CloseConnection(m_handle);
            m_handle = nullptr;
        }

        if (m_requestHeaders)
        {
            HTTPHeaders_Free(m_requestHeaders);
            m_requestHeaders = nullptr;
        }
    }

    std::string HttpRequest::GetPath()
    {
        return m_endpoint.Path();
    }

    void HttpRequest::SetPath(const std::string & path)
    {
        m_endpoint.Path(path);
    }

    void HttpRequest::AddQueryParameter(const std::string & name, const std::string & value)
    {
        if (name.empty())
        {
            throw std::invalid_argument("Query parameter name cannot be empty");
        }

        m_endpoint.AddQueryParameter(name, value);
    }

    void HttpRequest::SetRequestHeader(const std::string & name, const std::string & value)
    {
        if (name.empty())
        {
            throw std::invalid_argument("Request header name cannot be empty");
        }

        m_endpoint.SetHeader(name, value);
    }

    void HttpRequest::SetProxy(const std::string & host, int port, const std::string & username, const std::string & password)
    {
        m_endpoint.Proxy(host, port, username, password);
    }

    std::unique_ptr<HttpResponse> HttpRequest::SendRequest(HTTPAPI_REQUEST_TYPE type, const void *content, size_t contentSize)
    {
        std::string httpPath = m_endpoint.Path() + m_endpoint.QueryString();

        for (const auto& h : m_endpoint.Headers())
        {
            HTTPHeaders_AddHeaderNameValuePair(m_requestHeaders, h.first.c_str(), h.second.c_str());
        }

        HTTPHeaders_AddHeaderNameValuePair(m_requestHeaders, "Content-Length", std::to_string(contentSize).c_str());

        // Create the native HTTP instance
        if (m_handle == nullptr)
        {
            ProxyServerInfo proxy = m_endpoint.Proxy();
            m_handle = HTTPAPI_CreateConnection_Advanced(
                m_endpoint.Host().c_str(),
                m_endpoint.Port(),
                m_endpoint.IsSecure(),
                proxy.host.empty() ? nullptr : proxy.host.c_str(),
                proxy.port,
                proxy.username.empty() ? nullptr : proxy.username.c_str(),
                proxy.password.empty() ? nullptr : proxy.password.c_str());

            if (m_handle == nullptr)
            {
                throw std::runtime_error("Creating the HTTP request failed");
            }
        }
        
#ifdef SPEECHSDK_USE_OPENSSL
        if (m_endpoint.IsSecure())
        {
            int tls_version = OPTION_TLS_VERSION_1_2;
            if (HTTPAPI_SetOption(m_handle, OPTION_TLS_VERSION, &tls_version) != HTTPAPI_OK)
            {
                throw std::runtime_error("Could not set TLS 1.2 option");
            }

            bool disableDefaultVerifyPaths = m_endpoint.DisableDefaultVerifyPaths();
            bool disableCrlChecks = m_endpoint.DisableCrlChecks();
            std::string singleCert = m_endpoint.SingleTrustedCertificate();

            HTTPAPI_SetOption(m_handle, OPTION_DISABLE_DEFAULT_VERIFY_PATHS, &disableDefaultVerifyPaths);
            if (!singleCert.empty())
            {
                HTTPAPI_SetOption(m_handle, OPTION_TRUSTED_CERT, singleCert.c_str());
                HTTPAPI_SetOption(m_handle, OPTION_DISABLE_CRL_CHECK, &disableCrlChecks);
            }
        }
#endif

        auto response = std::make_unique<HttpResponse>();

        HTTPAPI_RESULT result = HTTPAPI_ExecuteRequest(
            m_handle,
            type,
            httpPath.c_str(),
            m_requestHeaders,
            (const unsigned char *)content,
            contentSize,
            &response->m_statusCode,
            response->m_responseHeaders,
            response->m_buffer);

        if (result != HTTPAPI_OK)
        {
            throw HttpException(result);
        }

        return response;
    }

} } } }
