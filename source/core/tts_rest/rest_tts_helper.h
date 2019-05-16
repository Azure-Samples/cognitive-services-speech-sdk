//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// rest_tts_helper.h: Implementation declarations for CSpxRestTtsHelper C++ class
//

#pragma once
#include <map>
#include <sstream>

#include "stdafx.h"
#include "string_utils.h"
#include "azure_c_shared_utility_string_token_wrapper.h"

#define HTTP_PROTOCOL "http://"
#define HTTPS_PROTOCOL "https://"
#define MIN_URL_PARSABLE_LENGTH 8

#define SSML_TEMPLATE "<speak version='1.0' xmlns='http://www.w3.org/2001/10/synthesis' xmlns:mstts='http://www.w3.org/2001/mstts' xmlns:emo='http://www.w3.org/2009/10/emotionml' xml:lang='%s'><voice name='%s'>%s</voice></speak>"
#define SSML_BUFFER_SIZE 0x10000

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {


typedef struct HttpUrl_Tag
{
    bool secure;
    std::string host;
    int port;
    std::string path;
    std::string query;
} HttpUrl;


class CSpxRestTtsHelper
{
public:

    static std::string itos(size_t i)
    {
        std::stringstream ss;
        ss << i;
        return ss.str();
    };

    static HttpUrl ParseHttpUrl(const std::string& urlStr)
    {
        HttpUrl url;

        if (urlStr.length() < MIN_URL_PARSABLE_LENGTH)
        {
            ThrowRuntimeError("Invalid url (unexpected length)");
        }

        // If url starts with "http://" (protocol), 'secure' shall be set to false
        if (PAL::strnicmp(urlStr.data(), HTTP_PROTOCOL, strlen(HTTP_PROTOCOL)) == 0)
        {
            url.secure = false;
        }
        // If url starts with "https://" (protocol), 'secure' shall be set to true
        else if (PAL::strnicmp(urlStr.data(), HTTPS_PROTOCOL, strlen(HTTPS_PROTOCOL)) == 0)
        {
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

        host_begin = (url.secure ? strlen(HTTPS_PROTOCOL) : strlen(HTTP_PROTOCOL));

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

                    // If the path component is present and 'http_url->path' ends up being NULL, the function shall fail
                    // If the path component is present and 'http_url->path_length' ends up being zero, the function shall fail
                    if (path == NULL || path_length == 0)
                    {
                        ThrowRuntimeError("Failed parsing http url path");
                    }
                    else
                    {
                        url.path = std::string(path, path_length);
                        path_parsed = true;
                    }
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

        return url;
    };

    static std::string BuildSsml(const std::string& text, const std::string& language, const std::string& voice)
    {
        std::map<std::string, std::string> languageToDefaultVoice = {
            { "ar-EG", "Microsoft Server Speech Text to Speech Voice (ar-EG, Hoda)" },
            { "ar-SA", "Microsoft Server Speech Text to Speech Voice (ar-SA, Naayf)" },
            { "bg-BG", "Microsoft Server Speech Text to Speech Voice (bg-BG, Ivan)" },
            { "ca-ES", "Microsoft Server Speech Text to Speech Voice (ca-ES, HerenaRUS)" },
            { "cs-CZ", "Microsoft Server Speech Text to Speech Voice (cs-CZ, Jakub)" },
            { "da-DK", "Microsoft Server Speech Text to Speech Voice (da-DK, HelleRUS)" },
            { "de-AT", "Microsoft Server Speech Text to Speech Voice (de-AT, Michael)" },
            { "de-CH", "Microsoft Server Speech Text to Speech Voice (de-CH, Karsten)" },
            { "de-DE", "Microsoft Server Speech Text to Speech Voice (de-DE, HeddaRUS)" },
            { "el-GR", "Microsoft Server Speech Text to Speech Voice (el-GR, Stefanos)" },
            { "en-AU", "Microsoft Server Speech Text to Speech Voice (en-AU, HayleyRUS)" },
            { "en-CA", "Microsoft Server Speech Text to Speech Voice (en-CA, HeatherRUS)" },
            { "en-GB", "Microsoft Server Speech Text to Speech Voice (en-GB, HazelRUS)" },
            { "en-IE", "Microsoft Server Speech Text to Speech Voice (en-IE, Sean)" },
            { "en-IN", "Microsoft Server Speech Text to Speech Voice (en-IN, PriyaRUS)" },
            { "en-US", "Microsoft Server Speech Text to Speech Voice (en-US, JessaRUS)" },
            { "es-ES", "Microsoft Server Speech Text to Speech Voice (es-ES, HelenaRUS)" },
            { "es-MX", "Microsoft Server Speech Text to Speech Voice (es-MX, HildaRUS)" },
            { "fi-FI", "Microsoft Server Speech Text to Speech Voice (fi-FI, HeidiRUS)" },
            { "fr-CA", "Microsoft Server Speech Text to Speech Voice (fr-CA, HarmonieRUS)" },
            { "fr-CH", "Microsoft Server Speech Text to Speech Voice (fr-CH, Guillaume)" },
            { "fr-FR", "Microsoft Server Speech Text to Speech Voice (fr-FR, HortenseRUS)" },
            { "he-IL", "Microsoft Server Speech Text to Speech Voice (he-IL, Asaf)" },
            { "hi-IN", "Microsoft Server Speech Text to Speech Voice (hi-IN, Kalpana)" },
            { "hr-HR", "Microsoft Server Speech Text to Speech Voice (hr-HR, Matej)" },
            { "hu-HU", "Microsoft Server Speech Text to Speech Voice (hu-HU, Szabolcs)" },
            { "id-ID", "Microsoft Server Speech Text to Speech Voice (id-ID, Andika)" },
            { "it-IT", "Microsoft Server Speech Text to Speech Voice (it-IT, LuciaRUS)" },
            { "ja-JP", "Microsoft Server Speech Text to Speech Voice (ja-JP, HarukaRUS)" },
            { "ko-KR", "Microsoft Server Speech Text to Speech Voice (ko-KR, HeamiRUS)" },
            { "ms-MY", "Microsoft Server Speech Text to Speech Voice (ms-MY, Rizwan)" },
            { "nb-NO", "Microsoft Server Speech Text to Speech Voice (nb-NO, HuldaRUS)" },
            { "nl-NL", "Microsoft Server Speech Text to Speech Voice (nl-NL, HannaRUS)" },
            { "pl-PL", "Microsoft Server Speech Text to Speech Voice (pl-PL, PaulinaRUS)" },
            { "pt-BR", "Microsoft Server Speech Text to Speech Voice (pt-BR, HeloisaRUS)" },
            { "pt-PT", "Microsoft Server Speech Text to Speech Voice (pt-PT, HeliaRUS)" },
            { "ro-RO", "Microsoft Server Speech Text to Speech Voice (ro-RO, Andrei)" },
            { "ru-RU", "Microsoft Server Speech Text to Speech Voice (ru-RU, EkaterinaRUS)" },
            { "sk-SK", "Microsoft Server Speech Text to Speech Voice (sk-SK, Filip)" },
            { "sl-SI", "Microsoft Server Speech Text to Speech Voice (sl-SI, Lado)" },
            { "sv-SE", "Microsoft Server Speech Text to Speech Voice (sv-SE, HedvigRUS)" },
            { "ta-IN", "Microsoft Server Speech Text to Speech Voice (ta-IN, Valluvar)" },
            { "te-IN", "Microsoft Server Speech Text to Speech Voice (te-IN, Chitra)" },
            { "th-TH", "Microsoft Server Speech Text to Speech Voice (th-TH, Pattara)" },
            { "tr-TR", "Microsoft Server Speech Text to Speech Voice (tr-TR, SedaRUS)" },
            { "vi-VN", "Microsoft Server Speech Text to Speech Voice (vi-VN, An)" },
            { "zh-CN", "Microsoft Server Speech Text to Speech Voice (zh-CN, HuihuiRUS)" },
            { "zh-HK", "Microsoft Server Speech Text to Speech Voice (zh-HK, TracyRUS)" },
            { "zh-TW", "Microsoft Server Speech Text to Speech Voice (zh-TW, HanHanRUS)" }
        };

        std::string chosenLanguage = language;
        std::string chosenVoice = voice;

        if (chosenLanguage.empty())
        {
            chosenLanguage = "en-US"; // Set default language to en-US
        }

        if (chosenVoice.empty())
        {
            // Set default voice based on language
            auto iter = languageToDefaultVoice.find(chosenLanguage);
            if (iter != languageToDefaultVoice.end())
            {
                chosenVoice = iter->second;
            }
            else
            {
                // Not found, use en-US default voice
                chosenVoice = "Microsoft Server Speech Text to Speech Voice (en-US, JessaRUS)";
            }
        }

        char* ssmlBuf = new char[SSML_BUFFER_SIZE];
        int ssmlLength = snprintf(ssmlBuf, SSML_BUFFER_SIZE, SSML_TEMPLATE, chosenLanguage.data(), chosenVoice.data(), XmlEncode(text).data());
        auto ssml = std::string(ssmlBuf, ssmlLength);
        delete[] ssmlBuf;

        return ssml;
    };

    static std::string XmlEncode(const std::string& text)
    {
        std::stringstream ss;
        for (size_t i = 0; i < text.length(); ++i)
        {
            char c = text[i];
            if (c == '&')
            {
                ss << "&amp;";
            }
            else if (c == '<')
            {
                ss << "&lt;";
            }
            else if (c == '>')
            {
                ss << "&gt;";
            }
            else if (c == '\'')
            {
                ss << "&apos;";
            }
            else if (c == '"')
            {
                ss << "&quot;";
            }
            else
            {
                ss << c;
            }
        }

        return ss.str();
    };

    static CancellationErrorCode HttpStatusCodeToCancellationErrorCode(int httpStatusCode)
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
};


} } } } // Microsoft::CognitiveServices::Speech::Impl
