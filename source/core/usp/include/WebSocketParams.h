//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// WebSocketParams.h: Defines the various parameters you need to pass to connect to a web socket
//

#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include "../uspcommon.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace USP {

    /// <summary>
    /// Defines the parameters for the web socket connection
    /// </summary>
    struct WebSocketParams
    {
    public:
        /// <summary>
        /// Creates a new instance
        /// </summary>
        /// <param name="connectionId">The connection ID to use</param>
        /// <param name="host">The host name</param>
        /// <param name="path">The path</param>
        /// <param name="isSecure">True if this is a secure connection</param>
        WebSocketParams(const std::string& connectionId, const std::string& host, const std::string& path, bool isSecure = true);

        /// <summary>
        /// Creates a new instance
        /// </summary>
        /// <param name="connectionId">The connection ID to use</param>
        /// <param name="host">The host name</param>
        /// <param name="path">The path</param>
        /// <param name="port">The port</param>
        /// <param name="isSecure">True if this is a secure connection</param>
        WebSocketParams(const std::string& connectionId, const std::string& host, const std::string& path, long port, bool isSecure);

        /// <summary>
        /// Gets the connection ID
        /// </summary>
        std::string ConnectionId() const { return m_connectionId; }

        /// <summary>
        /// Gets whether or not the connection should be secure
        /// </summary>
        bool IsSecure() const { return m_isSecure; }

        /// <summary>
        /// Gets the port to use
        /// </summary>
        long Port() const { return m_port; }

        /// <summary>
        /// Gets the web socket server host name only (e.g. www.host.com)
        /// </summary>
        std::string Host() const { return m_host; }

        /// <summary>
        /// Gets the path to that the web socket host is listening on (e.g. /sub/other)
        /// </summary>
        std::string Path() const { return m_path; }

        /// <summary>
        /// Gets the escaped query string to use. If there are query parameters, this will always
        /// start with '?'
        /// </summary>
        std::string QueryString() const;

        /// <summary>
        /// Gets a reference to the headers to include in the request
        /// </summary>
        const std::unordered_map<std::string, std::string>& Headers() const { return m_headers; }

        /// <summary>
        /// Gets the proxy server to use. If you enabled automatic proxy, this will parse and return
        /// the platform proxy information
        /// </summary>
        ProxyServerInfo proxyServerInfo() const;

        /// <summary>
        /// Gets the comma separated list of web socket protocols to use. This will be empty if none
        /// were set
        /// </summary>
        std::string webSocketProtocols() const;

        /// <summary>
        /// Gets the single trusted certificate to use. Will be an empty string if none was set
        /// </summary>
        std::string singleTrustedCertificate() const { return m_trustedCert; }

        /// <summary>
        /// Gets whether or not CRL checks are disabled
        /// </summary>
        bool disableCrlChecks() const { return m_disableCrlCheck; }

        /// <summary>
        /// Gets whether or not verification of default paths is disabled
        /// </summary>
        bool disableDefaultVerifyPaths() const { return m_disableDefaultVerifyPaths; }

        /// <summary>
        /// Sets the host to use
        /// </summary>
        /// <param name="host">The new host to set</param>
        /// <returns>Web socket parameters reference</returns>
        WebSocketParams& setHost(const std::string& host);

        /// <summary>
        /// Sets the path to use
        /// </summary>
        /// <param name="path">The path to web socket endpoint on the server</param>
        /// <returns>Web socket parameters reference</returns>
        WebSocketParams& setPath(const std::string& path);
        
        /// <summary>
        /// Adds some additional headers to the HTTP web socket upgrade request to the server
        /// </summary>
        /// <param name="name">The name of the header</param>
        /// <param name="value">The header value</param>
        /// <returns>Web socket parameters reference</returns>
        WebSocketParams& setHeader(const std::string& name, const std::string& value);

        /// <summary>
        /// Adds a query parameter to be included in the upgrade request to the server. You
        /// can set the same name multiple times if you need to. Doing so will result in
        /// the query parameter being set once, and comma separated value
        /// </summary>
        /// <param name="name">The query parameter name</param>
        /// <param name="value">The query value</param>
        /// <returns>Web socket parameters reference</returns>
        WebSocketParams& addQueryParameter(const std::string& name, const::std::string& value);

        /// <summary>
        /// Sets the value for query parameter to be included in the upgrade request to the
        /// server. If there were other values defined before, this will overwrite them with
        /// the new single value
        /// </summary>
        /// <param name="name">The query parameter name</param>
        /// <param name="value">The query value</param>
        /// <returns>Web socket parameters reference</returns>
        WebSocketParams& setQueryParameter(const std::string& name, const::std::string& value);

        /// <summary>
        /// Adds a web socket protocol to include in the headers when negotiating an upgrade request
        /// with the server. You don't always need to set this
        /// </summary>
        /// <param name="protocol">The protocol to include</param>
        /// <returns>Web socket parameters reference</returns>
        WebSocketParams& addWebSocketProtocol(const std::string& protocol);

        /// <summary>
        /// Call this to enable or disable automatically using the platform defined proxy. If this
        /// is true, any proxy you set using setProxy will be ignored
        /// </summary>
        /// <param name="autoProxy">True to enable, false to disable</param>
        /// <returns>Web socket parameters reference</returns>
        WebSocketParams& setAutomaticProxy(bool autoProxy);

        /// <summary>
        /// Sets the proxy to use. Calling this will disable automatic proxy
        /// </summary>
        /// <param name="proxyHost">The proxy host name</param>
        /// <param name="port">The port the proxy is listening on</param>
        /// <param name="username">(Optional) The username for the proxy</param>
        /// <param name="password">(Optional) The password for the proxy</param>
        /// <returns>Web socket parameters reference</returns>
        WebSocketParams& setProxy(const std::string& proxyHost, int port, const std::string& username = "", const std::string& password = "");

        /// <summary>
        /// When using OpenSSL only: sets a single trusted cert, optionally w/o CRL checks.
        /// This is meant to be used in a firewall setting with potential lack of
        /// CRLs(particularly on the leaf).
        /// </summary>
        /// <param name="trustedCert">The certificate to trust(PEM format)</param>
        /// <param name="disableCrlCheck">True to disable CRL check, false otherwise</param>
        /// <returns>Web socket parameters reference</returns>
        WebSocketParams& setSingleTrustedCert(const std::string& trustedCert, bool disableCrlCheck);

        /// <summary>
        /// Sets or not disable default verify paths is enabled
        /// </summary>
        /// <param name="disable">True to disable, false to enable</param>
        /// <returns>Web socket parameters reference</returns>
        WebSocketParams& setDisableDefaultVerifyPaths(const bool disable);

    private:
        bool m_isSecure;
        long m_port;
        std::string m_host;
        std::string m_path;
        std::unordered_map<std::string, std::vector<std::string>> m_queryParams;
        std::unordered_map<std::string, std::string> m_headers;
        std::vector<std::string> m_protocols;
        std::string m_connectionId;
        bool m_autoProxy;
        ProxyServerInfo m_proxy;
        std::string m_trustedCert;
        bool m_disableCrlCheck;
        bool m_disableDefaultVerifyPaths;
    };

} } } }
