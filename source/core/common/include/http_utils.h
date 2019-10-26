//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// rest_utils.h: a utility class for HTTP
//

#pragma once

#include "azure_c_shared_utility_string_token_wrapper.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

#define HTTP_PROTOCOL "http://"
#define HTTPS_PROTOCOL "https://"
#define WS_PROTOCOL "ws://"
#define WSS_PROTOCOL "wss://"
#define MIN_URL_PARSABLE_LENGTH 6

enum Protocol
{
    HTTP = 0,
    WebSocket = 1
};

typedef struct Url_Tag
{
    Protocol protocol;
    bool secure;
    std::string host;
    int port{ -1 };
    std::string path;
    std::string query;
} Url;

class HttpUtils
{
public:
    static const auto http_secure_connection_port_number = 443;
    static const auto http_non_secure_connection_port_number = 80;

    static Url ParseUrl(const std::string& urlStr)
    {
        Url url;

        if (urlStr.length() < MIN_URL_PARSABLE_LENGTH)
        {
            ThrowRuntimeError("Invalid url (unexpected length)");
        }

        // If url starts with "http://" (protocol), 'secure' shall be set to false
        if (PAL::strnicmp(urlStr.data(), HTTP_PROTOCOL, strlen(HTTP_PROTOCOL)) == 0)
        {
            url.protocol = Protocol::HTTP;
            url.secure = false;
        }
        // If url starts with "https://" (protocol), 'secure' shall be set to true
        else if (PAL::strnicmp(urlStr.data(), HTTPS_PROTOCOL, strlen(HTTPS_PROTOCOL)) == 0)
        {
            url.protocol = Protocol::HTTP;
            url.secure = true;
        }
        // If url starts with "ws://" (protocol), 'secure' shall be set to false
        else if (PAL::strnicmp(urlStr.data(), WS_PROTOCOL, strlen(WS_PROTOCOL)) == 0)
        {
            url.protocol = Protocol::WebSocket;
            url.secure = false;
        }
        // If url starts with "wss://" (protocol), 'secure' shall be set to true
        else if (PAL::strnicmp(urlStr.data(), WSS_PROTOCOL, strlen(WSS_PROTOCOL)) == 0)
        {
            url.protocol = Protocol::WebSocket;
            url.secure = true;
        }
        else
        {
            // If protocol cannot be identified in url, the function shall fail
            ThrowRuntimeError("Url protocol prefix not recognized");
        }

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

        host_begin = (url.protocol == Protocol::HTTP ? (url.secure ? strlen(HTTPS_PROTOCOL) : strlen(HTTP_PROTOCOL)) : (url.secure ? strlen(WSS_PROTOCOL) : strlen(WS_PROTOCOL)));

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
                // If after 'host' the 'port_delimiter' occurs (not preceeded by 'path_delimiter' or 'query_delimiter') the number that follows shall be parsed and stored in 'port'
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
                // If after 'host' or the port number the 'path_delimiter' occurs (not preceeded by 'query_delimiter') the following pointer address shall be stored in 'path'
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
            url.port = url.secure ? http_secure_connection_port_number : http_non_secure_connection_port_number;
        }

        return url;
    };
};

} } } } // Microsoft::CognitiveServices::Speech::Impl
