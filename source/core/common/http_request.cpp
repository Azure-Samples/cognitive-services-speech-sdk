//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#include "stdafx.h"
#include <sstream>
#include <azure_c_shared_utility_urlencode_wrapper.h>
#include "httpapi_platform_proxy.h"
#include "http_request.h"
#include "http_response.h"

#ifdef SPEECHSDK_USE_OPENSSL
    #include <azure_c_shared_utility/shared_util_options.h>
#endif

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

    static std::string UrlEncode(const std::string& value)
    {
        STRING_HANDLE string = nullptr;
        std::string encoded;
        try
        {
            string = URL_EncodeString(value.data());
            encoded = std::string(STRING_c_str(string));
        }
        catch (...)
        {
            // ignore
        }

        STRING_delete(string);
        return encoded;
    }

    HttpRequest::HttpRequest(const std::string& host)
        : m_host(host),
        m_path(std::string()),
        m_query(std::map<std::string, std::vector<std::string>>()),
        m_handle(HTTPAPI_CreateConnection_With_Platform_Proxy(host.data())),
        m_requestHeaders(HTTPHeaders_Alloc())
    {
        if (host.empty())
        {
            Term();
            throw std::invalid_argument("You cannot specify an empty host");
        }
        else if (!m_requestHeaders || !m_handle)
        {
            Term();
            throw std::bad_alloc();
        }

        SetRequestHeader("Host", m_host);
    }

    HttpRequest::~HttpRequest()
    {
        Term();
    }

    void HttpRequest::Term()
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

    void HttpRequest::AddQueryParameter(const std::string & key, const std::string & value)
    {
        m_query[key].push_back(value);
    }

    void HttpRequest::SetRequestHeader(const std::string & key, const std::string & value)
    {
        HTTPHeaders_AddHeaderNameValuePair(m_requestHeaders, key.data(), value.data());
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
                std::string encodedKey = UrlEncode(kvp.first);

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
                    httpPath += UrlEncode(value);
                }
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

} } } }
