//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#include "stdafx.h"
#include <sstream>
#include <azure_c_shared_utility_urlencode_wrapper.h>
#include <azure_c_shared_utility_httpapi_wrapper.h>
#include "http_request.h"
#include "http_response.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

    constexpr size_t MAX_SERVICE_ERROR_RESPONSE_LENGTH = 2048;

    HttpResponse::HttpResponse()
        : m_statusCode(0), m_responseHeaders(HTTPHeaders_Alloc()), m_buffer(BUFFER_new())
    {
        if (!m_buffer || !m_responseHeaders)
        {
            Term();
            throw std::bad_alloc();
        }
    }

    HttpResponse::~HttpResponse()
    {
        Term();
    }

    void HttpResponse::Term()
    {
        if (m_buffer)
        {
            BUFFER_delete(m_buffer);
            m_buffer = nullptr;
        }

        if (m_responseHeaders)
        {
            HTTPHeaders_Free(m_responseHeaders);
            m_responseHeaders = nullptr;
        }
    }

    bool HttpResponse::IsSuccess()
    {
        return m_statusCode >= 200 && m_statusCode < 300;
    }

    void HttpResponse::EnsureSuccess()
    {
        if (!IsSuccess())
        {
            std::string errorMessage;

            // let's try to read the response from the service if there is any and include it in the exception message
            try
            {
                std::ostringstream oss;
                oss << "HTTP " << m_statusCode << ": " << ReadContentAsString(&MAX_SERVICE_ERROR_RESPONSE_LENGTH);
                errorMessage = oss.str();

                SPX_TRACE_ERROR("HTTP request failed: '%s'", errorMessage.c_str());
            }
            catch (...)
            {
                // ignore errors. Just return the HTTP status code
            }

            if (errorMessage.empty())
            {
                throw HttpException(m_statusCode);
            }
            else
            {
                throw HttpException(HTTPAPI_RESULT::HTTPAPI_OK, m_statusCode, errorMessage);
            }
        }
    }

    unsigned int HttpResponse::GetStatusCode()
    {
        return m_statusCode;
    }

    std::string HttpResponse::GetHeader(const std::string & key)
    {
        return HTTPHeaders_FindHeaderValue(m_responseHeaders, key.data());
    }

    std::string HttpResponse::ReadContentAsString()
    {
        return ReadContentAsString(nullptr);
    }

    std::string HttpResponse::ReadContentAsString(const size_t * length)
    {
        unsigned char *ptr = BUFFER_u_char(m_buffer);
        if (ptr == nullptr)
        {
            return std::string();
        }

        size_t size = BUFFER_length(m_buffer);
        if (size == 0)
        {
            return std::string();
        }
        else if (length != nullptr && size > *length)
        {
            size = *length; // WARNING: This may break UTF8 character sequences!
        }

        return std::string((const char*)ptr, size);
    }

} } } }
