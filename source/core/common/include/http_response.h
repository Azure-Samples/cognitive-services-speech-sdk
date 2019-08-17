//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#pragma once

#include "http_exception.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

    class HttpRequest;

    class HttpResponse
    {
        friend class HttpRequest;

    public:
        HttpResponse();
        ~HttpResponse();

        bool IsSuccess();
        void EnsureSuccess();

        unsigned int GetStatusCode();

        std::string GetHeader(const std::string& key);
        std::string ReadContentAsString();

    private:
        void Term();

    private:
        unsigned int m_statusCode;
        HTTP_HEADERS_HANDLE m_responseHeaders;
        BUFFER_HANDLE m_buffer;
    };

} } } } // Microsoft::CognitiveServices::Speech::Impl
