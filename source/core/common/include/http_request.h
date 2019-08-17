//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#pragma once

#include <map>
#include <string>
#include <azure_c_shared_utility/httpapi.h>
#include "http_exception.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

    class HttpResponse;

    class HttpRequest
    {
    public:
        HttpRequest(const std::string& host);
        ~HttpRequest();

        std::string GetPath();
        void SetPath(const std::string& path);

        void AddQueryParameter(const std::string& key, const std::string& value);

        void SetRequestHeader(const std::string& key, const std::string& value);

        std::unique_ptr<HttpResponse> SendRequest(HTTPAPI_REQUEST_TYPE type) {
            return SendRequest(type, nullptr, 0);
        }

        std::unique_ptr<HttpResponse> SendRequest(HTTPAPI_REQUEST_TYPE type, const void *content, size_t contentSize);

    private:
        void Term();

    private:
        std::string m_host;
        std::string m_path;
        std::map<std::string, std::vector<std::string>> m_query;
        HTTP_HANDLE m_handle;
        HTTP_HEADERS_HANDLE m_requestHeaders;
    };

} } } } // Microsoft::CognitiveServices::Speech::Impl
