//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#include "stdafx.h"
#include "azure_c_shared_utility_strings_wrapper.h"
#include "azure_c_shared_utility_string_token_wrapper.h"
#include "azure_c_shared_utility_urlencode_wrapper.h"
#include "http_utils.h"
#include "string_utils.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

    using StringUtils = PAL::StringUtils;

    template<typename T>
    using deleted_unique_ptr = std::unique_ptr<T, std::function<void(T*)>>;

    const size_t MIN_URL_PARSABLE_LENGTH = 6;
    const int16_t HTTP_SECURE_PORT = 443;
    const int16_t HTTP_NON_SECURE_PORT = 80;


    Url HttpUtils::ParseUrl(const std::string& rawUrlStr)
    {
        Url url;
        std::string urlStr = StringUtils::Trim(rawUrlStr);

        if (urlStr.length() < MIN_URL_PARSABLE_LENGTH)
        {
            ThrowRuntimeError("Invalid url (unexpected length)");
        }

        url.scheme = HttpUtils::ParseScheme(urlStr.c_str());

        size_t host_begin;

        const char* port_delimiter = ":";
        const char* path_delimiter = "/";
        const char* query_delimiter = "?";
        const char* delimiters1[3];
        const char* delimiters2[1];

        size_t delimiter_count = 3;

        const char** current_delimiters = delimiters1;
        const char* previous_delimiter = NULL;

        bool host_parsed = false;
        bool port_parsed = false;
        bool path_parsed = false;
        bool query_parsed = false;

        STRING_TOKEN_HANDLE token;

        delimiters1[0] = port_delimiter;
        delimiters1[1] = path_delimiter;
        delimiters1[2] = query_delimiter;
        delimiters2[0] = query_delimiter;

        host_begin = strlen(HttpUtils::SchemePrefix(url.scheme));

        token = StringToken_GetFirst(urlStr.data() + host_begin, urlStr.length() - host_begin, current_delimiters, delimiter_count);

        if (token == NULL)
        {
            ThrowRuntimeError("Failed getting first url token");
        }
        else
        {
            do
            {
                const char* current_delimiter = (char*)StringToken_GetDelimiter(token);
                if (previous_delimiter == NULL && !host_parsed && !port_parsed && !path_parsed && !query_parsed)
                {
                    // The pointer to the token starting right after 'protocol' (in the 'url' string) shall be stored in 'host'
                    auto host = StringToken_GetValue(token);

                    // The length from 'host' up to the first occurrence of either ":" ('port_delimiter'), "/" ('path_delimiter'), "?" ('query_delimiter') or '\0' shall be stored in 'host_length'
                    auto host_length = StringToken_GetLength(token);

                    // If 'host' ends up being NULL, the function shall fail
                    // If 'host_length' ends up being zero, the function shall fail
                    if (host == NULL || host_length == 0)
                    {
                        ThrowRuntimeError("Failed parsing http url host");
                    }
                    else
                    {
                        url.host = std::string(host, host_length);
                        host_parsed = true;
                    }
                }
                // If after 'host' the 'port_delimiter' occurs (not preceded by 'path_delimiter' or 'query_delimiter') the number that follows shall be parsed and stored in 'port'
                else if (previous_delimiter == port_delimiter && host_parsed && !port_parsed && !path_parsed && !query_parsed)
                {
                    const char* port_str = StringToken_GetValue(token);
                    size_t port_length = StringToken_GetLength(token);

                    // If the port number fails to be parsed, the function shall fail
                    if (port_str == NULL || port_length == 0)
                    {
                        ThrowRuntimeError("Failed parsing http url port");
                    }
                    else
                    {
                        char port_copy[10];
                        (void)memset(port_copy, 0, sizeof(char) * 10);
                        (void)memcpy(port_copy, port_str, port_length);
                        url.port = atoi(port_copy);
                        port_parsed = true;
                    }
                }
                // If after 'host' or the port number the 'path_delimiter' occurs (not preceded by 'query_delimiter') the following pointer address shall be stored in 'path'
                else if (previous_delimiter == path_delimiter && host_parsed && !path_parsed && !query_parsed)
                {
                    auto path = StringToken_GetValue(token);

                    // The length from 'path' up to the first occurrence of either 'query_delimiter' or '\0' shall be stored in 'path_length'
                    auto path_length = StringToken_GetLength(token);

                    if (path != NULL && path_length > 0)
                    {
                        url.path = std::string(path, path_length);
                    }
                    // else it's the root path '/' and url.path is empty

                    path_parsed = true;
                }
                // Next if the 'query_delimiter' occurs the following pointer address shall be stored in 'query'
                else if (previous_delimiter == query_delimiter && current_delimiter == NULL && host_parsed && !query_parsed)
                {
                    auto query = StringToken_GetValue(token);

                    // The length from 'query' up to '\0' shall be stored in 'query_length'
                    auto query_length = StringToken_GetLength(token);

                    // If the query component is present and 'http_url->query' ends up being NULL, the function shall fail
                    // If the query component is present and 'http_url->query_length' ends up being zero, the function shall fail
                    if (query == NULL || query_length == 0)
                    {
                        ThrowRuntimeError("Failed parsing http url query");
                    }
                    else
                    {
                        url.query = std::string(query, query_length);
                        query_parsed = true;
                    }
                }
                else
                {
                    ThrowRuntimeError("Failed parsing http url (format not recognized)");
                }

                if (current_delimiter == path_delimiter)
                {
                    current_delimiters = delimiters2;
                    delimiter_count = 1;
                }

                previous_delimiter = current_delimiter;
            } while (StringToken_GetNext(token, current_delimiters, delimiter_count));

            StringToken_Destroy(token);
        }

        if (url.port < 0)
        {
            url.port = url.isSecure() ? HTTP_SECURE_PORT : HTTP_NON_SECURE_PORT;
        }

        return url;
    }


    std::string HttpUtils::UrlEscape(const std::string& value)
    {
        if (value.empty())
        {
            return value;
        }

        deleted_unique_ptr<STRING_TAG> escaped(URL_EncodeString(value.c_str()), &STRING_delete);
        if (escaped != nullptr)
        {
            return std::string(STRING_c_str(escaped.get()));
        }
        else
        {
            return std::string();
        }
    }

    std::string HttpUtils::UrlUnescape(const std::string & value)
    {
        if (value.empty())
        {
            return value;
        }

        deleted_unique_ptr<STRING_TAG> unescaped(URL_DecodeString(value.c_str()), &STRING_delete);
        if (unescaped != nullptr)
        {
            return std::string(STRING_c_str(unescaped.get()));
        }
        else
        {
            return std::string();
        }
    }

    std::unordered_map<std::string, std::vector<std::string>> HttpUtils::ParseQueryString(const std::string & queryString)
    {
        bool first = true;
        std::unordered_map<std::string, std::vector<std::string>> parsed;

        std::vector<std::string> parameters = StringUtils::Tokenize(queryString, "&");
        for (const std::string& kvp : parameters)
        {
            auto parts = StringUtils::Tokenize(kvp, "=");
            if (parts.size() > 0 && parts.size() <= 2)
            {
                std::string key = parts[0];
                if (first && !key.empty() && key[0] == '?')
                {
                    key = UrlUnescape(key.substr(1));
                }
                else
                {
                    key = UrlUnescape(key);
                }

                std::string value;
                if (parts.size() > 1)
                {
                    value = UrlUnescape(parts[1]);
                }

                parsed[key].push_back(value);
            }

            first = false;
        }

        return parsed;
    }

    const char * HttpUtils::SchemePrefix(UriScheme scheme)
    {
        switch (scheme)
        {
            case UriScheme::HTTP: return HTTP;
            case UriScheme::HTTPS: return HTTPS;
            case UriScheme::WS: return WS;
            case UriScheme::WSS: return WSS;
            default:
                ThrowRuntimeError("Unsupported scheme");
                return nullptr;
        }
    }

    /// <summary>
    /// Determines the URI scheme from the specified prefix string
    /// </summary>
    /// <param name="str">The null terminated string to examine (e.g. https://)</param>
    /// <returns>The equivalent URI scheme</returns>
    UriScheme HttpUtils::ParseScheme(const char * str)
    {
        // If url starts with "http://" (protocol), 'secure' shall be set to false
        if (PAL::strnicmp(str, HTTP, strlen(HTTP)) == 0)
        {
            return UriScheme::HTTP;
        }
        // If url starts with "https://" (protocol), 'secure' shall be set to true
        else if (PAL::strnicmp(str, HTTPS, strlen(HTTPS)) == 0)
        {
            return UriScheme::HTTPS;
        }
        // If url starts with "ws://" (protocol), 'secure' shall be set to false
        else if (PAL::strnicmp(str, WS, strlen(WS)) == 0)
        {
            return UriScheme::WS;
        }
        // If url starts with "wss://" (protocol), 'secure' shall be set to true
        else if (PAL::strnicmp(str, WSS, strlen(WSS)) == 0)
        {
            return UriScheme::WSS;
        }
        else
        {
            // If protocol cannot be identified in url, the function shall fail
            ThrowRuntimeError("Url protocol prefix not recognized");
            return UriScheme::HTTPS; // should never get here
        }
    }

    CancellationErrorCode HttpStatusCodeToCancellationErrorCode(int httpStatusCode)
    {
        if (httpStatusCode < 400)
        {
            return CancellationErrorCode::NoError;
        }

        CancellationErrorCode errorCode = CancellationErrorCode::NoError;
        switch (httpStatusCode)
        {
        case 401:
            errorCode = CancellationErrorCode::AuthenticationFailure;
            break;

        case 400:
            errorCode = CancellationErrorCode::BadRequest;
            break;

        case 429:
            errorCode = CancellationErrorCode::TooManyRequests;
            break;

        case 403:
            errorCode = CancellationErrorCode::Forbidden;
            break;

        case 408:
        case 504:
            errorCode = CancellationErrorCode::ServiceTimeout;
            break;

        case 404:
        case 409:
        case 415:
        case 500:
        case 501:
        case 502:
        case 505:
        case 506:
        case 507:
        case 509:
        case 510:
        case 600:
            errorCode = CancellationErrorCode::ServiceError;
            break;

        case 503:
            errorCode = CancellationErrorCode::ServiceUnavailable;
            break;

        default:
            errorCode = CancellationErrorCode::ConnectionFailure;
            break;
        }

        return errorCode;
    }
} } } }
