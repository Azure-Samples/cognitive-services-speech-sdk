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

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

#define HTTP_PROTOCOL "http://"
#define HTTPS_PROTOCOL "https://"
#define WS_PROTOCOL "ws://"
#define WSS_PROTOCOL "wss://"

/// <summary>
/// URL protocols
/// </summary>
enum class Protocol
{
    HTTP = 0,
    WebSocket = 1
};


/// <summary>
/// The parts of a URL
/// </summary>
typedef struct Url_Tag
{
    /// <summary>
    /// The protocol of the URL
    /// </summary>
    Protocol protocol;
    /// <summary>
    /// Whether or not a secure connection should be used
    /// </summary>
    bool secure;
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
};

} } } } // Microsoft::CognitiveServices::Speech::Impl
