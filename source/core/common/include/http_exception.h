//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#pragma once

#include <stdexcept>
#include <map>
#include <string>
#include <azure_c_shared_utility_httpapi_wrapper.h>

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

    class HttpException : public std::runtime_error
    {
    public:
        explicit HttpException(HTTPAPI_RESULT result);
        explicit HttpException(unsigned int httpStatusCode);
        explicit HttpException(HTTPAPI_RESULT result, unsigned int httpStatusCode, const char *msg);
        explicit HttpException(HTTPAPI_RESULT result, unsigned int httpStatusCode, const std::string& msg);

        virtual size_t result() const;
        virtual unsigned int statusCode() const;

    protected:
        static std::string GenerateHttpApiResultError(HTTPAPI_RESULT result);
        static std::string GenerateHttpStatusCodeError(unsigned int httpStatusCode);

    protected:
        size_t m_result;
        unsigned int m_statusCode;
    };

} } } } // Microsoft::CognitiveServices::Speech::Impl
