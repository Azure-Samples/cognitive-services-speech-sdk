//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#pragma once

#include <map>
#include <string>
#include <azure_c_shared_utility_httpapi_wrapper.h>
#include <http_exception.h>
#include <http_endpoint_info.h>

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

    class HttpResponse;

    /// <summary>
    /// A wrapper around the Azure C shared library code to allow sending HTTPS requests
    /// </summary>
    class HttpRequest
    {
    public:
        /// <summary>
        /// Creates a new instance
        /// </summary>
        /// <param name="host">The host to send requests to</param>
        /// <param name="isSecure">True if the request should use a secure connection.</param>
        HttpRequest(const std::string& host, bool isSecure = true)
            : HttpRequest(host, isSecure ? 443 : 80, isSecure)
        {}

        /// <summary>
        /// Creates a new instance
        /// </summary>
        /// <param name="host">The host to send requests to</param>
        /// <param name="port">The port on the host to connect to</param>
        /// <param name="isSecure">True if the request should use a secure connection.</param>
        HttpRequest(const std::string& host, int port, bool isSecure);

        /// <summary>
        /// Creates a new instance
        /// </summary>
        /// <param name="host">The endpoint information</param>
        HttpRequest(const HttpEndpointInfo& endpoint);

        /// <summary>
        /// Destructor
        /// </summary>
        ~HttpRequest();

        /// <summary>
        /// Gets the path to the HTTP resource on the server (e.g. sub/path)
        /// </summary>
        std::string GetPath();

        /// <summary>
        /// Sets the path to use
        /// </summary>
        /// <param name="path">The path to HTTP endpoint on the server</param>
        void SetPath(const std::string& path);

        /// <summary>
        /// Adds a query parameter to be included in the request to the server. You can
        /// set the same name multiple times if you need to. Doing so will result in the
        /// query parameter being set once, and comma separated value
        /// </summary>
        /// <param name="name">The query parameter name</param>
        /// <param name="value">The query value</param>
        void AddQueryParameter(const std::string& name, const std::string& value);

        /// <summary>
        /// Adds some additional headers to the HTTP request to the server
        /// </summary>
        /// <param name="name">The name of the header</param>
        /// <param name="value">The header value</param>
        void SetRequestHeader(const std::string& name, const std::string& value);

        /// <summary>
        /// Sets the proxy to use for the connection
        /// </summary>
        /// <param name="host">The proxy host name</param>
        /// <param name="port">The port the proxy is listening on</param>
        /// <param name="username">(Optional) The username for the proxy</param>
        /// <param name="password">(Optional) The password for the proxy</param>
        void SetProxy(const std::string& host, int port, const std::string& username = "", const std::string& password = "");

        /// <summary>
        /// Sends an HTTP request without a payload
        /// </summary>
        /// <param name="type">The type of HTTP request to send (e.g. GET)</param>
        /// <returns>The HTTP response</returns>
        /// <exception cref="HttpException">If the connection to the host failed, or another error
        /// was encountered. Exceptions are NOT thrown for non success HTTP status (e.g. 400, 500)</exception>
        std::unique_ptr<HttpResponse> SendRequest(HTTPAPI_REQUEST_TYPE type)
        {
            return SendRequest(type, nullptr, 0);
        }

        /// <summary>
        /// Sends an HTTP request with a payload
        /// </summary>
        /// <param name="type">The type of HTTP request to send (e.g. POST)</param>
        /// <param name="content">Pointer to raw byte data to send</param>
        /// <param name="contentSize">The number of bytes to send</param>
        /// <returns>The HTTP response</returns>
        /// <exception cref="HttpException">If the connection to the host failed, or another error
        /// was encountered. Exceptions are NOT thrown for non success HTTP status (e.g. 400, 500)</exception>
        std::unique_ptr<HttpResponse> SendRequest(HTTPAPI_REQUEST_TYPE type, const void *content, size_t contentSize);

    private:
        HttpEndpointInfo m_endpoint;
        HTTP_HANDLE m_handle;
        HTTP_HEADERS_HANDLE m_requestHeaders;
    };

} } } } // Microsoft::CognitiveServices::Speech::Impl
