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

    /// <summary>
    /// The HTTP response to an HTTP request
    /// </summary>
    class HttpResponse
    {
        friend class HttpRequest;

    public:
        /// <summary>
        /// Creates a new instance
        /// </summary>
        HttpResponse();

        /// <summary>
        /// Destructor
        /// </summary>
        ~HttpResponse();

        /// <summary>
        /// Determines whether or not the HTTP response has a success status code (i.e. 2XX)
        /// </summary>
        /// <returns></returns>
        bool IsSuccess();

        /// <summary>
        /// Throws an exception if the HTTP response doesn't have a success status code
        /// </summary>
        /// <exception cref="HttpException">If the status code is outside of the range 200-299 (inclusive)</exception>
        void EnsureSuccess();

        /// <summary>
        /// Gets the HTTP status code
        /// </summary>
        /// <returns>Status code e.g. 200, 404</returns>
        unsigned int GetStatusCode();

        /// <summary>
        /// Retrieves a header value from the response
        /// </summary>
        /// <param name="name">The name of the header</param>
        /// <returns>The header value. If that header was not present, an empty string will be returned</returns>
        std::string GetHeader(const std::string& name);

        /// <summary>
        /// Reads the content of the HTTP response as a string
        /// </summary>
        /// <returns>The content as a string</returns>
        std::string ReadContentAsString();

    private:
        void Term();
        std::string ReadContentAsString(const size_t * maxLength);

    private:
        unsigned int m_statusCode;
        HTTP_HEADERS_HANDLE m_responseHeaders;
        BUFFER_HANDLE m_buffer;
    };

} } } } // Microsoft::CognitiveServices::Speech::Impl
