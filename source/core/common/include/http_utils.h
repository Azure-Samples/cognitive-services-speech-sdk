//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// rest_utils.h: a utility class for HTTP
//

#pragma once

#include <string>
#include <vector>
#include <unordered_map>

#include "speechapi_cxx_enums.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

/// <summary>
/// URI schemes
/// </summary>
enum class UriScheme
{
    HTTPS = 0,
    WSS,
    HTTP,
    WS
};


/// <summary>
/// The parts of a URL
/// </summary>
typedef struct Url_Tag
{
    /// <summary>
    /// The scheme of the URL
    /// </summary>
    UriScheme scheme;

    /// <summary>
    /// Whether or not a secure connection should be used
    /// </summary>
    bool isSecure() const
    {
        switch (scheme)
        {
            case UriScheme::HTTPS:
            case UriScheme::WSS:
                return true;

            case UriScheme::HTTP:
            case UriScheme::WS:
                return false;

            default:
                throw std::runtime_error("Could not determine if the unsupported URI scheme is secure");
        }
    }

    /// <summary>
    /// The host name (e.g. dev.microsofttranslator.com)
    /// </summary>
    std::string host;
    /// <summary>
    /// The port you are connecting to
    /// </summary>
    int port{ -1 };
    /// <summary>
    /// The path
    /// </summary>
    std::string path;
    /// <summary>
    /// The query string
    /// </summary>
    std::string query;
} Url;


/// <summary>
/// Utilities for working with HTTP requests
/// </summary>
class HttpUtils
{
public:
    /// <summary>
    /// Parses a URL into its parts
    /// </summary>
    /// <param name="urlStr">The URL to parse</param>
    /// <returns>The parsed URL</returns>
    /// <exception cref="ExceptionWithCallStack">If the URL could not be parsed or is not supported</exception>
    static Url ParseUrl(const std::string& urlStr);

    /// <summary>
    /// URL escapes a value. So for example "Hello? how are you" would become
    /// "Hello%3f%20how%20are%20you"
    /// </summary>
    /// <param name="value">The value to escape</param>
    /// <returns>The escaped value</returns>
    static std::string UrlEscape(const std::string& value);

    /// <summary>
    /// URL unescapes the string. For example "Test%3FYes%3Dtrue" would become
    /// "Test?Yes=true"
    /// </summary>
    /// <param name="value">The value to unescape</param>
    /// <returns>The unescaped value</returns>
    static std::string UrlUnescape(const std::string& value);

    /// <summary>
    /// Parses the query string into name value(s) pairs
    /// </summary>
    /// <param name="queryString">The query string to parse</param>
    /// <returns>The name value(s) pairs</returns>
    static std::unordered_map<std::string, std::vector<std::string>> ParseQueryString(const std::string& queryString);

    /// <summary>
    /// Checks whether or not the specified port is valid
    /// </summary>
    /// <param name="port">The port to validate</param>
    static bool IsValidPort(int port)
    {
        return port > 0 && port <= 65535;
    }

    /// <summary>
    /// Gets the prefix to use for the URI scheme e.g. https://
    /// </summary>
    /// <param name="scheme">The URI scheme</param>
    /// <returns>The scheme prefix to use</returns>
    static const char * SchemePrefix(UriScheme scheme);

    /// <summary>
    /// Determines the URI scheme from the specified prefix string
    /// </summary>
    /// <param name="str">The null terminated string to examine (e.g. https://)</param>
    /// <returns>The equivalent URI scheme</returns>
    static UriScheme ParseScheme(const char * str);

private:
    static constexpr auto HTTP = "http://";
    static constexpr auto HTTPS = "https://";
    static constexpr auto WS = "ws://";
    static constexpr auto WSS = "wss://";
};

CancellationErrorCode HttpStatusCodeToCancellationErrorCode(int httpStatusCode);

} } } } // Microsoft::CognitiveServices::Speech::Impl
