//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#include "stdafx.h"
#include <sstream>
#include <azure_c_shared_utility_urlencode_wrapper.h>
#include "httpapi_platform_proxy.h"
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
        m_secure(isSecure),
        m_host(host),
        m_port(port),
        m_path(),
        m_query(),
        m_proxyHost(),
        m_proxyPort(),
        m_proxyUsername(),
        m_proxyPassword(),
        m_handle(),
        m_requestHeaders()
    {
        ValidatePort(port);
        if (host.empty())
        {
            throw std::invalid_argument("You cannot specify an empty host");
        }

        m_requestHeaders = HTTPHeaders_Alloc();
        if (m_requestHeaders == nullptr)
        {
            throw std::bad_alloc();
        }
        
        SetRequestHeader("Host", m_host);
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
        return m_path;
    }

    void HttpRequest::SetPath(const std::string & path)
    {
        m_path = path;
    }

    void HttpRequest::AddQueryParameter(const std::string & name, const std::string & value)
    {
        if (name.empty())
        {
            throw std::invalid_argument("Query parameter name cannot be empty");
        }

        m_query[name].push_back(value);
    }

    void HttpRequest::SetRequestHeader(const std::string & name, const std::string & value)
    {
        if (name.empty())
        {
            throw std::invalid_argument("Request header name cannot be empty");
        }

        HTTPHeaders_AddHeaderNameValuePair(m_requestHeaders, name.data(), value.data());
    }

    void HttpRequest::SetProxy(const std::string & host, int port, const std::string & username, const std::string & password)
    {
        if (false == host.empty())
        {
            ValidatePort(port);
        }

        m_proxyHost = host;
        m_proxyPort = port;
        m_proxyUsername = username;
        m_proxyPassword = password;
    }

    std::unique_ptr<HttpResponse> HttpRequest::SendRequest(HTTPAPI_REQUEST_TYPE type, const void *content, size_t contentSize)
    {
        // add remaining standard HTTP headers
        SetRequestHeader("Content-Length", std::to_string(contentSize));

        std::string httpPath = m_path;

        // Generate query string
        if (m_query.size() > 0)
        {
            bool first = true;
            httpPath += "?";

            for (auto const& kvp : m_query)
            {
                std::string encodedKey = HttpUtils::UrlEscape(kvp.first);

                const std::vector<std::string> &values = kvp.second;
                for (const std::string &value : values)
                {
                    if (first)
                    {
                        first = false;
                    }
                    else
                    {
                        httpPath += "&";
                    }

                    httpPath += encodedKey;
                    httpPath += "=";
                    httpPath += HttpUtils::UrlEscape(value);
                }
            }
        }

        if (m_handle == nullptr)
        {
            if (m_proxyHost.empty())
            {
                m_handle = HTTPAPI_CreateConnection_With_Platform_Proxy(m_host.c_str(), m_port, m_secure);
            }
            else
            {
                m_handle = HTTPAPI_CreateConnection_Advanced(
                    m_host.c_str(),
                    m_port,
                    m_secure,
                    m_proxyHost.c_str(),
                    m_proxyPort,
                    m_proxyUsername.c_str(),
                    m_proxyPassword.c_str());
            }

            if (m_handle == nullptr)
            {
                throw std::runtime_error("Creating the HTTP request failed");
            }
        }

#ifdef SPEECHSDK_USE_OPENSSL
        int tls_version = OPTION_TLS_VERSION_1_2;
        if (HTTPAPI_SetOption(m_handle, OPTION_TLS_VERSION, &tls_version) != HTTPAPI_OK)
        {
            throw std::runtime_error("Could not set TLS 1.2 option");
        }
#endif

        auto response = std::make_unique<HttpResponse>();

        HTTPAPI_RESULT result = HTTPAPI_ExecuteRequest(
            m_handle,
            type,
            httpPath.data(),
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

    void HttpRequest::ValidatePort(int port)
    {
        if (false == HttpUtils::IsValidPort(port))
            throw std::invalid_argument("The port is not valid. (0 < port <= 65535)");
    }

} } } }
