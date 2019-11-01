//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// WebSocketParams.cpp: The implementation for the web socket parameters
//

#include <string.h>
#include <errno.h>
#include "WebSocketParams.h"
#include "azure_c_shared_utility_platform_wrapper.h"
#include "azure_c_shared_utility_httpapi_wrapper.h"
#include <azure_c_shared_utility_urlencode_wrapper.h>
#include "http_utils.h"
#include "string_utils.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace USP {

    using HttpUtils = Microsoft::CognitiveServices::Speech::Impl::HttpUtils;
    using StringUtils = PAL::StringUtils;

    const size_t MAX_HOSTNAME_PORT_STRING_LEN = MAX_HOSTNAME_LEN + 1 + 5;
    const size_t MAX_USERNAME_PASSWORD_STRING_LEN = MAX_USERNAME_LEN + MAX_PASSWORD_LEN + 1;

    WebSocketParams::WebSocketParams(const std::string& connectionId, const std::string& host, const std::string& path, bool isSecure)
        : WebSocketParams(connectionId, host, path, isSecure ? 443 : 80, isSecure)
    {
    }

    WebSocketParams::WebSocketParams(const std::string& connectionId, const std::string& host, const std::string& path, long port, bool isSecure) :
        m_isSecure(isSecure),
        m_port(port),
        m_host(host),
        m_path(path),
        m_queryParams(),
        m_headers(),
        m_protocols(),
        m_connectionId(connectionId),
        m_autoProxy(true),
        m_proxy(),
        m_trustedCert(),
        m_disableCrlCheck(false),
        m_disableDefaultVerifyPaths(false)
    {
    }

    std::string WebSocketParams::QueryString() const
    {
        std::string query;
        bool first = true;

        for (auto const& kvp : m_queryParams)
        {
            std::string encodedKey = HttpUtils::UrlEscape(kvp.first);

            const std::vector<std::string> &values = kvp.second;
            for (const std::string &value : values)
            {
                if (first)
                {
                    first = false;
                    query += "?";
                }
                else
                {
                    query += "&";
                }

                query += encodedKey;
                query += "=";
                query += HttpUtils::UrlEscape(value);
            }
        }

        return query;
    }

    ProxyServerInfo WebSocketParams::proxyServerInfo() const
    {
        if (!m_autoProxy)
        {
            return m_proxy;
        }

        ProxyServerInfo platform;

        const char* hostPort = nullptr;
        const char* usernamePassword = nullptr;

        platform_get_http_proxy(&hostPort, &usernamePassword);

        auto parts = StringUtils::Tokenize(hostPort, strnlen(hostPort, MAX_HOSTNAME_PORT_STRING_LEN), ": ");
        if (parts.size() != 2)
        {
            return platform;
        }

        int port = (int)strtol(parts[1].c_str(), NULL, 10);
        if (errno == ERANGE || port <= 0)
        {
            return platform;
        }

        platform.host = parts[0];
        platform.port = port;

        parts = StringUtils::Tokenize(usernamePassword, strnlen(usernamePassword, MAX_USERNAME_PASSWORD_STRING_LEN), ": ");
        if (parts.size() != 2)
        {
            return platform;
        }

        platform.username = parts[0];
        platform.password = parts[1];

        return platform;
    }

    std::string WebSocketParams::webSocketProtocols() const
    {
        std::string protocols;
        bool first = true;
        for (size_t i = 0; i < m_protocols.size(); i++, first = false)
        {
            if (!first)
            {
                protocols += ", ";
            }

            protocols += m_protocols[i];
        }

        return protocols;
    }

    WebSocketParams & WebSocketParams::setHost(const std::string & host)
    {
        m_host = host;
        return *this;
    }

    WebSocketParams & WebSocketParams::setPath(const std::string & path)
    {
        m_path = path;
        return *this;
    }

    WebSocketParams& WebSocketParams::setHeader(const std::string& name, const std::string& value)
    {
        m_headers[name] = value;
        return *this;
    }

    WebSocketParams& WebSocketParams::addQueryParameter(const std::string& name, const::std::string& value)
    {
        m_queryParams[name].push_back(value);
        return *this;
    }

    WebSocketParams & WebSocketParams::setQueryParameter(const std::string & name, const::std::string & value)
    {
        auto& values = m_queryParams[name];
        values.clear();
        values.push_back(value);
        return *this;
    }

    WebSocketParams& WebSocketParams::addWebSocketProtocol(const std::string& protocol)
    {
        m_protocols.push_back(protocol);
        return *this;
    }

    WebSocketParams& WebSocketParams::setAutomaticProxy(bool autoProxy)
    {
        m_autoProxy = autoProxy;
        return *this;
    }

    WebSocketParams& WebSocketParams::setProxy(const std::string& proxyHost, int port, const std::string& username, const std::string& password)
    {
        m_autoProxy = false;
        m_proxy.host = proxyHost;
        m_proxy.port = port;
        m_proxy.username = username;
        m_proxy.password = password;
        return *this;
    }

    WebSocketParams & WebSocketParams::setSingleTrustedCert(const std::string & trustedCert, bool disableCrlCheck)
    {
        m_trustedCert = trustedCert;
        m_disableCrlCheck = disableCrlCheck;
        return *this;
    }

    WebSocketParams & WebSocketParams::setDisableDefaultVerifyPaths(const bool disable)
    {
        m_disableDefaultVerifyPaths = disable;
        return *this;
    }

} } } }
