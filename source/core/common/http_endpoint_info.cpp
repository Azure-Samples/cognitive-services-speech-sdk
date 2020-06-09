//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#include <sstream>
#include <algorithm>
#include <http_utils.h>
#include <string_utils.h>
#include <azure_c_shared_utility_platform_wrapper.h>
#include <azure_c_shared_utility_httpapi_wrapper.h>
#include <http_endpoint_info.h>

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

    using StringUtils = PAL::StringUtils;
    using namespace std;


    const size_t MAX_HOSTNAME_PORT_STRING_LEN = MAX_HOSTNAME_LEN + 1 + 5;
    const size_t MAX_USERNAME_PASSWORD_STRING_LEN = MAX_USERNAME_LEN + MAX_PASSWORD_LEN + 1;
    const int HTTP_SECURE_PORT = 443;
    const int HTTP_NON_SECURE_PORT = 80;


    HttpEndpointInfo::HttpEndpointInfo() :
        m_endpoint(),
        m_queryParams(),
        m_autoProxy(true),
        m_webSocketProtocols()
    {
        m_endpoint.scheme = UriScheme::HTTPS;
        m_endpoint.port = m_endpoint.isSecure() ? HTTP_SECURE_PORT : HTTP_NON_SECURE_PORT;
        m_endpoint.path = "/";
    }

    bool HttpEndpointInfo::IsValid() const
    {
        return !m_endpoint.host.empty()
            && HttpUtils::IsValidPort(m_endpoint.port);
    }

    std::string HttpEndpointInfo::EndpointUrl() const
    {
        if (!IsValid())
        {
            throw std::logic_error("Endpoint is not valid");
        }

        return GenerateEndpointUrl(GeneratePathAndQueryString(GenerateQueryString()));
    }

    HttpEndpointInfo& HttpEndpointInfo::EndpointUrl(const std::string & endpointUrl)
    {
        Url parsed = HttpUtils::ParseUrl(endpointUrl);

        Scheme(parsed.scheme)
            .Host(parsed.host)
            .Port(parsed.port)
            .Path(parsed.path)
            .QueryString(parsed.query);

        return *this;
    }

    UriScheme HttpEndpointInfo::Scheme() const
    {
        return m_endpoint.scheme;
    }

    HttpEndpointInfo & HttpEndpointInfo::Scheme(UriScheme scheme)
    {
        bool oldSecure = m_endpoint.isSecure();
        bool oldIsDefaultPort = IsDefaultPort();
        m_endpoint.scheme = scheme;

        // update the port if it was not set, or was set to the default
        if (!HttpUtils::IsValidPort(m_endpoint.port)
            || (oldSecure ^ m_endpoint.isSecure() && oldIsDefaultPort))
        {
            m_endpoint.port = m_endpoint.isSecure() ? HTTP_SECURE_PORT : HTTP_NON_SECURE_PORT;
        }

        return *this;
    }

    bool HttpEndpointInfo::IsSecure() const
    {
        return m_endpoint.isSecure();
    }

    std::string HttpEndpointInfo::Host()
    {
        return m_endpoint.host;
    }

    HttpEndpointInfo & HttpEndpointInfo::Host(const std::string & hostname)
    {
        std::string trimmed = StringUtils::Trim(hostname);

        if (trimmed.empty())
        {
            throw std::invalid_argument("Host name cannot be empty");
        }
        else if (trimmed.length() > MAX_HOSTNAME_FQDN_LEN)
        {
            throw std::out_of_range("Host name is too long");
        }

        m_endpoint.host = std::move(trimmed);
        return *this;
    }

    int HttpEndpointInfo::Port() const
    {
        return m_endpoint.port < 0 ? -1 : m_endpoint.port;
    }

    HttpEndpointInfo & HttpEndpointInfo::Port(int port)
    {
        if (!HttpUtils::IsValidPort(port))
        {
            throw std::invalid_argument("Port is not valid");
        }

        m_endpoint.port = port;
        return *this;
    }

    bool HttpEndpointInfo::IsDefaultPort() const
    {
        switch (m_endpoint.scheme)
        {
            case UriScheme::HTTP:
            case UriScheme::WS:
                return m_endpoint.port == HTTP_NON_SECURE_PORT;

            case UriScheme::HTTPS:
            case UriScheme::WSS:
                return m_endpoint.port == HTTP_SECURE_PORT;

            default:
                throw std::runtime_error("Could not determine the default port for the unsupported URI scheme");
        }
    }

    std::string HttpEndpointInfo::Path() const
    {
        return m_endpoint.path;
    }

    HttpEndpointInfo & HttpEndpointInfo::Path(const std::string & path)
    {
        // make sure to add the / at the start if not already there
        std::ostringstream oss;
        oss << '/';

        StringUtils::Trim(path, oss, [](char c, bool atStart) { return atStart && c == '/'; });

        m_endpoint.path = oss.str();
        return *this;
    }

    std::string HttpEndpointInfo::QueryString() const
    {
        return GenerateQueryString();
    }

    HttpEndpointInfo & HttpEndpointInfo::QueryString(const std::string & queryString)
    {
        m_queryParams.clear();
        m_queryParams = HttpUtils::ParseQueryString(queryString);
        return *this;
    }

    HttpEndpointInfo & HttpEndpointInfo::AddQueryParameter(const std::string & name, const std::string & value)
    {
        if (name.empty())
        {
            throw std::invalid_argument("Query parameter name cannot be empty");
        }

        m_queryParams[name].push_back(value);
        return *this;
    }

    HttpEndpointInfo & HttpEndpointInfo::SetQueryParameter(const std::string & name, const::std::string & value)
    {
        if (name.empty())
        {
            throw std::invalid_argument("Query parameter name cannot be empty");
        }

        auto& values = m_queryParams[name];
        values.clear();
        values.push_back(value);
        return *this;
    }

    bool HttpEndpointInfo::AutomaticProxy()
    {
        return m_autoProxy;
    }

    HttpEndpointInfo & HttpEndpointInfo::AutomaticProxy(bool autoProxy)
    {
        m_autoProxy = autoProxy;
        return *this;
    }

    ProxyServerInfo HttpEndpointInfo::Proxy() const
    {
        return m_autoProxy ? GetPlatformProxy() : m_endpoint.proxy;
    }

    HttpEndpointInfo & HttpEndpointInfo::Proxy(const std::string & proxyHost, int port, const std::string & username, const std::string & password)
    {
        if (proxyHost.length() > MAX_HOSTNAME_LEN)
        {
            throw std::out_of_range("Host name is too long");
        }
        else if (!proxyHost.empty() && !HttpUtils::IsValidPort(port))
        {
            throw std::invalid_argument("The proxy port is invalid");
        }

        AutomaticProxy(false);
        m_endpoint.proxy.host = proxyHost;
        m_endpoint.proxy.port = port < 0 ? -1 : port;
        m_endpoint.proxy.username = username;
        m_endpoint.proxy.password = password;
        return *this;
    }

    const std::unordered_map<std::string, std::string>& HttpEndpointInfo::Headers() const
    {
        return m_endpoint.headers;
    }

    HttpEndpointInfo & HttpEndpointInfo::SetHeader(const std::string & name, const std::string & value)
    {
        if (name.empty())
        {
            throw std::invalid_argument("Header name cannot be empty");
        }

        m_endpoint.headers[name] = value;
        return *this;
    }

    std::string HttpEndpointInfo::WebSocketProtocols() const
    {
        bool first = true;
        std::ostringstream oss;

        for (size_t i = 0; i < m_webSocketProtocols.size(); i++, first = false)
        {
            if (!first)
            {
                oss << ", ";
            }

            oss << m_webSocketProtocols[i];
        }

        return oss.str();
    }

    size_t HttpEndpointInfo::WebSocketProtocolsSize() const
    {
        return m_webSocketProtocols.size();
    }

    HttpEndpointInfo & HttpEndpointInfo::AddWebSocketProtocol(const std::string & protocol)
    {
        if (protocol.empty())
        {
            throw std::invalid_argument("Web socket protocol cannot be empty");
        }

        auto iter = find(m_webSocketProtocols.begin(), m_webSocketProtocols.end(), protocol);
        if (iter == m_webSocketProtocols.end())
        {
            m_webSocketProtocols.push_back(protocol);
        }

        return *this;
    }

    std::string HttpEndpointInfo::SingleTrustedCertificate() const
    {
        return m_endpoint.singleTrustedCert;
    }

    HttpEndpointInfo & HttpEndpointInfo::SingleTrustedCertificate(const std::string & trustedCert, bool disableCrlCheck)
    {
        m_endpoint.singleTrustedCert = trustedCert;
        m_endpoint.disableCrlChecks = disableCrlCheck;
        return *this;
    }

    bool HttpEndpointInfo::DisableCrlChecks() const
    {
        return m_endpoint.disableCrlChecks;
    }

    bool HttpEndpointInfo::DisableDefaultVerifyPaths() const
    {
        return m_endpoint.disableDefaultVerifyPaths;
    }

    HttpEndpointInfo & HttpEndpointInfo::DisableDefaultVerifyPaths(const bool disable)
    {
        m_endpoint.disableDefaultVerifyPaths = disable;
        return *this;
    }

    const HttpEndpointInfo::Internals HttpEndpointInfo::GetInternals() const
    {
        // create copy
        HttpEndpointInfo::Internals copy(m_endpoint);

        // update internal state
        copy.query = GenerateQueryString();
        copy.pathAndQuery = GeneratePathAndQueryString(copy.query);
        copy.url = GenerateEndpointUrl(copy.path);

        copy.proxy = Proxy();
        copy.webSocketProtcols = WebSocketProtocols();
        copy.webSocketProtocolCount = m_webSocketProtocols.size();

        return copy;
    }

    std::string HttpEndpointInfo::GenerateQueryString() const
    {
        bool first = true;
        std::ostringstream oss;

        for (auto const& kvp : m_queryParams)
        {
            std::string encodedKey = HttpUtils::UrlEscape(kvp.first);

            const std::vector<std::string> &values = kvp.second;
            for (const std::string &value : values)
            {
                if (first)
                {
                    first = false;
                    oss << "?";
                }
                else
                {
                    oss << "&";
                }

                oss << encodedKey;
                if (!value.empty())
                {
                    oss << "=" << HttpUtils::UrlEscape(value);
                }
            }
        }

        return oss.str();
    }

    std::string HttpEndpointInfo::GeneratePathAndQueryString(const std::string & queryString) const
    {
        std::ostringstream oss;

        if (m_endpoint.path.empty() || m_endpoint.path[0] != '/')
        {
            oss << '/';
        }

        oss << m_endpoint.path;

        if (!queryString.empty() && queryString[0] != '?')
        {
            oss << '?';
        }

        oss << queryString;

        return oss.str();
    }

    std::string HttpEndpointInfo::GenerateEndpointUrl(const std::string & pathAndQueryString) const
    {
        if (!IsValid())
        {
            throw std::logic_error("Endpoint is not valid");
        }

        std::ostringstream oss;
        oss << HttpUtils::SchemePrefix(m_endpoint.scheme)
            << m_endpoint.host;

        if (pathAndQueryString.empty() || pathAndQueryString[0] != '/')
        {
            oss << '/';
        }

        oss << pathAndQueryString;

        return oss.str();
    }

    ProxyServerInfo HttpEndpointInfo::GetPlatformProxy() const
    {
        ProxyServerInfo proxy;

        const char* hostPort = nullptr;
        const char* usernamePassword = nullptr;

        platform_get_http_proxy(&hostPort, &usernamePassword);

        auto parts = StringUtils::Tokenize(hostPort, strnlen(hostPort, MAX_HOSTNAME_PORT_STRING_LEN), ": ");
        if (parts.size() != 2)
        {
            return proxy;
        }

        int port = (int)strtol(parts[1].c_str(), NULL, 10);
        if (errno == ERANGE || port <= 0)
        {
            return proxy;
        }

        proxy.host = parts[0];
        proxy.port = port;

        parts = StringUtils::Tokenize(usernamePassword, strnlen(usernamePassword, MAX_USERNAME_PASSWORD_STRING_LEN), ": ");
        if (parts.size() != 2)
        {
            return proxy;
        }

        proxy.username = parts[0];
        proxy.password = parts[1];

        return proxy;
    }

}}}} // Microsoft::CognitiveServices::Speech::Impl

