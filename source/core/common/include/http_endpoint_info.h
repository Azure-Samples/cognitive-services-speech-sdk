//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#pragma once

#define MAX_HOSTNAME_FQDN_LEN 253 // The maximum length of a host's FQDN in ASCII. See https://devblogs.microsoft.com/oldnewthing/20120412-00/?p=7873

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <http_utils.h>
#include <proxy_server_info.h>

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

    /// <summary>
    /// Defines the parameters for an HTTP endpoint
    /// </summary>
    struct HttpEndpointInfo
    {
        /// <summary>
        /// Constructor
        /// </summary>
        HttpEndpointInfo();

        /// <summary>
        /// Gets whether or not we have a valid endpoint URL defined.
        /// </summary>
        /// <returns>True if valid, false otherwise</returns>
        bool IsValid() const;

        /// <summary>
        /// Gets the absolute URL of the endpoint
        /// </summary>
        /// <returns>The URL of the endpoint</returns>
        /// <exception cref="std::logic_error">Thrown if we don't have a valid endpoint defined</exception>
        std::string EndpointUrl() const;

        /// <summary>
        /// Sets the absolute URL. This will override all the existing values for the URL including
        /// any set query parameters.
        /// </summary>
        /// <param name="endpointUrl">The absolute endpoint URL to set e.g. https://www.contoso.com/path/subpath?key=value </param>
        /// <returns>A reference to this instance</returns>
        HttpEndpointInfo& EndpointUrl(const std::string& endpointUrl);

        /// <summary>
        /// Gets the HTTP protocol for the endpoint. Please note that this will change the port used
        /// if it is currently set to the defaults.
        /// </summary>
        /// <returns>The HTTP protocol</returns>
        UriScheme Scheme() const;

        /// <summary>
        /// Sets the HTTP protocol for the endpoint
        /// </summary>
        /// <param name="scheme">The URI scheme</param>
        /// <returns>A reference to this instance</returns>
        HttpEndpointInfo& Scheme(UriScheme scheme);

        /// <summary>
        /// Gets whether or not the connection should be secure
        /// </summary>
        bool IsSecure() const;
        
        /// <summary>
        /// Gets the host name only (e.g. www.contoso.com)
        /// </summary>
        std::string Host();

        /// <summary>
        /// Sets the host name
        /// </summary>
        /// <param name="hostname">The host name to use e.g. www.contoso.com</param>
        /// <exception cref="std::invalid_argument">If host name is empty</exception>
        /// <returns>A reference to this instance</returns>
        HttpEndpointInfo& Host(const std::string& hostname);

        /// <summary>
        /// Gets the endpoint port
        /// </summary>
        /// <returns>The current port set, or -1 if no port was set</returns>
        int Port() const;

        /// <summary>
        /// Sets the endpoint port.
        /// </summary>
        /// <param name="port">The port to set</param>
        /// <exception cref="std::invalid_argument">If the port is invalid</exception>
        /// <returns>A reference to this instance</returns>
        HttpEndpointInfo& Port(int port);

        /// <summary>
        /// Gets whether or not the port specified is the default for the current URI scheme
        /// </summary>
        /// <returns>True if default, false otherwise</returns>
        bool IsDefaultPort() const;

        /// <summary>
        /// Gets the path that the host is listening on (e.g. /sub/other)
        /// </summary>
        std::string Path() const;

        /// <summary>
        /// Sets the path that the host is listening on
        /// </summary>
        /// <param name="path">The path to set.</param>
        /// <returns>A reference to this instance</returns>
        HttpEndpointInfo& Path(const std::string& path);

        /// <summary>
        /// Gets the escaped query string to use. If there are query parameters, this will always
        /// start with '?'
        /// </summary>
        std::string QueryString() const;

        /// <summary>
        /// Sets the query string to use
        /// </summary>
        /// <param name="queryString">The query string</param>
        /// <returns>A reference to this instance</returns>
        HttpEndpointInfo& QueryString(const std::string& queryString);

        /// <summary>
        /// Adds a query parameter to be included in the upgrade request to the server. You
        /// can set the same name multiple times if you need to. Doing so will result in
        /// the query parameter being set once with comma separated values
        /// </summary>
        /// <param name="name">The query parameter name</param>
        /// <param name="value">The query value</param>
        /// <returns>A reference to this instance</returns>
        HttpEndpointInfo& AddQueryParameter(const std::string& name, const std::string& value);

        /// <summary>
        /// Sets the value for query parameter to be included in the upgrade request to the
        /// server. If there were other values defined before, this will overwrite them with
        /// the new single value
        /// </summary>
        /// <param name="name">The query parameter name</param>
        /// <param name="value">The query value</param>
        /// <returns>A reference to this instance</returns>
        HttpEndpointInfo& SetQueryParameter(const std::string& name, const::std::string& value);

        /// <summary>
        /// Gets whether or not we are using the platform defined proxy.
        /// </summary>
        /// <returns>True to use the platform proxy, false otherwise</returns>
        bool AutomaticProxy();

        /// <summary>
        /// Call this to enable or disable automatically using the platform defined proxy. If this
        /// is true, any proxy you set using the SetProxy method will be ignored. Default is true.
        /// </summary>
        /// <param name="autoProxy">True to enable, false to disable</param>
        /// <returns>A reference to this instance</returns>
        HttpEndpointInfo& AutomaticProxy(bool autoProxy);

        /// <summary>
        /// Gets the proxy server to use. If you enabled automatic proxy, this will parse and return
        /// the platform proxy information
        /// </summary>
        ProxyServerInfo Proxy() const;

        /// <summary>
        /// Sets the proxy to use. Calling this will disable automatic proxy
        /// </summary>
        /// <param name="proxyHost">The proxy host name. Set this to an empty string to disable using a proxy.</param>
        /// <param name="port">The port the proxy is listening on</param>
        /// <param name="username">(Optional) The username for the proxy</param>
        /// <param name="password">(Optional) The password for the proxy</param>
        /// <returns>A reference to this instance</returns>
        HttpEndpointInfo& Proxy(const std::string& proxyHost, int port, const std::string& username = "", const std::string& password = "");

        /// <summary>
        /// Gets a reference to the headers to include in the request
        /// </summary>
        const std::unordered_map<std::string, std::string>& Headers() const;

        /// <summary>
        /// Adds a header to be sent with the HTTP request
        /// </summary>
        /// <param name="name">The name of the header</param>
        /// <param name="value">The header value</param>
        /// <returns>A reference to this instance</returns>
        HttpEndpointInfo& SetHeader(const std::string& name, const std::string& value);

        /// <summary>
        /// Gets the comma separated list of web socket protocols to use. This will be empty if none
        /// were set
        /// </summary>
        std::string WebSocketProtocols() const;

        /// <summary>
        /// Gets the number of web socket protocols defined
        /// </summary>
        size_t WebSocketProtocolsSize() const;

        /// <summary>
        /// Adds a web socket protocol to include in the headers when negotiating an upgrade request
        /// with the server. You don't always need to set this
        /// </summary>
        /// <param name="protocol">The protocol to include</param>
        /// <returns>A reference to this instance</returns>
        HttpEndpointInfo& AddWebSocketProtocol(const std::string& protocol);

        /// <summary>
        /// Gets the single trusted certificate to use.
        /// </summary>
        /// <returns>The trusted cert, or an empty string if none was set.</returns>
        std::string SingleTrustedCertificate() const;
        
        /// <summary>
        /// When using OpenSSL only: sets a single trusted cert, optionally w/o Certificate
        /// Revocation List (CRL) checks. This is meant to be used in a firewall setting with
        /// potential lack of CRLs (particularly on the leaf).
        /// </summary>
        /// <param name="trustedCert">The certificate to trust (PEM format)</param>
        /// <param name="disableCrlCheck">True to disable CRL check, false otherwise</param>
        /// <returns>A reference to this instance</returns>
        HttpEndpointInfo& SingleTrustedCertificate(const std::string& trustedCert, bool disableCrlCheck);

        /// <summary>
        /// Gets whether or not CRL checks are disabled
        /// </summary>
        /// <returns>True if disabled, false otherwise</returns>
        bool DisableCrlChecks() const;

        /// <summary>
        /// Gets whether or not verification of default paths is disabled
        /// </summary>
        /// <returns>True if disabled, false otherwise</returns>
        bool DisableDefaultVerifyPaths() const;

        /// <summary>
        /// Sets whether or not disable default verify paths
        /// </summary>
        /// <param name="disable">True to disable, false to enable</param>
        /// <returns>A reference to this instance</returns>
        HttpEndpointInfo& DisableDefaultVerifyPaths(const bool disable);

        /// <summary>
        /// The internal structure that can be used when we need to hold a reference to the various strings
        /// when we make calls to C code and pass const char * pointers
        /// </summary>
        struct Internals : public Url
        {
            std::string url;
            std::string pathAndQuery;
            std::string query;
            ProxyServerInfo proxy;
            size_t webSocketProtocolCount;
            std::string webSocketProtcols;

            std::string singleTrustedCert;
            bool disableCrlChecks;
            bool disableDefaultVerifyPaths;
            std::unordered_map<std::string, std::string> headers;
        };

        /// <summary>
        /// Gets the internals of this HTTP endpoint info
        /// </summary>
        const Internals GetInternals() const;

    protected:
        std::string GenerateQueryString() const;
        std::string GeneratePathAndQueryString(const std::string& queryString) const;
        std::string GenerateEndpointUrl(const std::string& pathAndQueryString) const;
        ProxyServerInfo GetPlatformProxy() const;

    private:
        Internals m_endpoint;

        std::unordered_map<std::string, std::vector<std::string>> m_queryParams;
        bool m_autoProxy = true;
        std::vector<std::string> m_webSocketProtocols;
    };

}}}} // Microsoft::CognitiveServices::Speech::Impl
