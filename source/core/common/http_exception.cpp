//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#include "stdafx.h"
#include <http_exception.h>
#include <azure_c_shared_utility_httpapi_wrapper.h>

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

    HttpException::HttpException(HTTPAPI_RESULT result) : HttpException(result, 0, GenerateHttpApiResultError(result))
    {}

    HttpException::HttpException(unsigned int httpStatusCode) : HttpException(HTTPAPI_RESULT::HTTPAPI_OK, httpStatusCode, GenerateHttpStatusCodeError(httpStatusCode))
    {}

    HttpException::HttpException(HTTPAPI_RESULT result, unsigned int httpStatusCode, const char *msg)
        : std::runtime_error(msg), m_result(result), m_statusCode(httpStatusCode)
    {}

    HttpException::HttpException(HTTPAPI_RESULT result, unsigned int httpStatusCode, const std::string& msg)
        : std::runtime_error(msg), m_result(result), m_statusCode(httpStatusCode)
    {}

    size_t HttpException::result() const { return m_result; }
    unsigned int HttpException::statusCode() const { return m_statusCode; }

    std::string HttpException::GenerateHttpApiResultError(HTTPAPI_RESULT result)
    {
        std::string error("Failed to execute HTTP request. HTTPAPI result code = ");
        error += ENUM_TO_STRING(HTTPAPI_RESULT, result);
        error += ".";

        return error;
    }

    std::string HttpException::GenerateHttpStatusCodeError(unsigned int httpStatusCode)
    {
        std::string error("Request failed with HTTP ");
        error += std::to_string(httpStatusCode);
        error += ".";

        return error;
    }

} } } }
