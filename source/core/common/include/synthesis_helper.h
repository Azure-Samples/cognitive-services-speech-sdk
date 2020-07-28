//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// rest_tts_helper.h: Implementation declarations for CSpxSynthesisHelper C++ class
//

#pragma once
#include <map>
#include <sstream>

#include "stdafx.h"
#include "string_utils.h"
#include "http_utils.h"
#include "property_id_2_name_map.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

constexpr auto TTS_COGNITIVE_SERVICE_HOST_SUFFIX = ".tts.speech.microsoft.com";
constexpr auto TTS_COGNITIVE_SERVICE_URL_PATH = "/cognitiveservices/v1";
constexpr auto USER_AGENT = "SpeechSDK";

constexpr uint32_t RIFF_MARKER = 0x46464952;
constexpr uint32_t WAVE_MARKER = 0x45564157;
constexpr uint32_t FMT_MARKER = 0x20746d66;
constexpr uint32_t DATA_MARKER = 0x61746164;
constexpr uint32_t EVNT_MARKER = 0x544e5645;

template<typename T>
void buffer_write(uint8_t** buffer_cursor, T value)
{
    auto buf = *buffer_cursor;
    for (size_t i = 0; i < sizeof(T); ++i)
    {
        *buf = static_cast<uint8_t>((value >> (i * 8)) & 0xff);
        ++buf;
    }
    *buffer_cursor = buf;
}

struct RIFFHDR
{
    uint32_t _id;
    uint32_t _len;              /* file length less header */
    uint32_t _type;            /* should be "WAVE" */

    RIFFHDR(uint32_t length)
    {
        _id = RIFF_MARKER;
        _type = WAVE_MARKER;
        _len = length;
    }
};

struct BLOCKHDR
{
    uint32_t _id;              /* should be "fmt " or "data" */
    uint32_t _len;              /* block size less header */

    BLOCKHDR(uint32_t length)
    {
        _id = FMT_MARKER;
        _len = length;
    }
};

struct DATAHDR
{
    uint32_t _id;               /* should be "fmt " or "data" */
    uint32_t _len;              /* block size less header */

    DATAHDR(uint32_t length)
    {
        _id = DATA_MARKER;
        _len = length;
    }
};

struct EVNTHDR
{
    uint32_t _id;               /* should be "EVNT" */
    uint32_t _len;              /* block size less header */

    EVNTHDR(uint32_t length)
    {
        _id = EVNT_MARKER;
        _len = length;
    }
};

class CSpxSynthesisHelper
{
public:

    static std::string itos(size_t i)
    {
        std::stringstream ss;
        ss << i;
        return ss.str();
    };

    static std::string BuildSsml(const std::string& text, const std::shared_ptr<ISpxNamedProperties>& properties)
    {
        using tuple_type = std::tuple<const char *, const char *>;
        constexpr std::array<tuple_type, 49> languageToDefaultVoice{ {
            tuple_type{ "ar-EG", "(ar-EG, Hoda)" },
            tuple_type{ "ar-SA", "(ar-SA, Naayf)" },
            tuple_type{ "bg-BG", "(bg-BG, Ivan)" },
            tuple_type{ "ca-ES", "(ca-ES, HerenaRUS)" },
            tuple_type{ "cs-CZ", "(cs-CZ, Jakub)" },
            tuple_type{ "da-DK", "(da-DK, HelleRUS)" },
            tuple_type{ "de-AT", "(de-AT, Michael)" },
            tuple_type{ "de-CH", "(de-CH, Karsten)" },
            tuple_type{ "de-DE", "(de-DE, HeddaRUS)" },
            tuple_type{ "el-GR", "(el-GR, Stefanos)" },
            tuple_type{ "en-AU", "(en-AU, HayleyRUS)" },
            tuple_type{ "en-CA", "(en-CA, HeatherRUS)" },
            tuple_type{ "en-GB", "(en-GB, HazelRUS)" },
            tuple_type{ "en-IE", "(en-IE, Sean)" },
            tuple_type{ "en-IN", "(en-IN, PriyaRUS)" },
            tuple_type{ "en-US", "(en-US, AriaRUS)" },
            tuple_type{ "es-ES", "(es-ES, HelenaRUS)" },
            tuple_type{ "es-MX", "(es-MX, HildaRUS)" },
            tuple_type{ "fi-FI", "(fi-FI, HeidiRUS)" },
            tuple_type{ "fr-CA", "(fr-CA, HarmonieRUS)" },
            tuple_type{ "fr-CH", "(fr-CH, Guillaume)" },
            tuple_type{ "fr-FR", "(fr-FR, HortenseRUS)" },
            tuple_type{ "he-IL", "(he-IL, Asaf)" },
            tuple_type{ "hi-IN", "(hi-IN, Kalpana)" },
            tuple_type{ "hr-HR", "(hr-HR, Matej)" },
            tuple_type{ "hu-HU", "(hu-HU, Szabolcs)" },
            tuple_type{ "id-ID", "(id-ID, Andika)" },
            tuple_type{ "it-IT", "(it-IT, LuciaRUS)" },
            tuple_type{ "ja-JP", "(ja-JP, HarukaRUS)" },
            tuple_type{ "ko-KR", "(ko-KR, HeamiRUS)" },
            tuple_type{ "ms-MY", "(ms-MY, Rizwan)" },
            tuple_type{ "nb-NO", "(nb-NO, HuldaRUS)" },
            tuple_type{ "nl-NL", "(nl-NL, HannaRUS)" },
            tuple_type{ "pl-PL", "(pl-PL, PaulinaRUS)" },
            tuple_type{ "pt-BR", "(pt-BR, HeloisaRUS)" },
            tuple_type{ "pt-PT", "(pt-PT, HeliaRUS)" },
            tuple_type{ "ro-RO", "(ro-RO, Andrei)" },
            tuple_type{ "ru-RU", "(ru-RU, EkaterinaRUS)" },
            tuple_type{ "sk-SK", "(sk-SK, Filip)" },
            tuple_type{ "sl-SI", "(sl-SI, Lado)" },
            tuple_type{ "sv-SE", "(sv-SE, HedvigRUS)" },
            tuple_type{ "ta-IN", "(ta-IN, Valluvar)" },
            tuple_type{ "te-IN", "(te-IN, Chitra)" },
            tuple_type{ "th-TH", "(th-TH, Pattara)" },
            tuple_type{ "tr-TR", "(tr-TR, SedaRUS)" },
            tuple_type{ "vi-VN", "(vi-VN, An)" },
            tuple_type{ "zh-CN", "(zh-CN, HuihuiRUS)" },
            tuple_type{ "zh-HK", "(zh-HK, TracyRUS)" },
            tuple_type{ "zh-TW", "(zh-TW, HanHanRUS)" }
        } };

        // Set default language to en-US
        std::string chosenLanguage = properties->GetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_SynthLanguage), "en-US");
        std::string chosenVoice = properties->GetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_SynthVoice), "");

        if (LanguageAutoDetectionEnabled(properties))
        {
            chosenLanguage = "en-US";
        }
        else if (chosenVoice.empty())
        {
            constexpr auto prefix = "Microsoft Server Speech Text to Speech Voice ";
            chosenVoice.reserve(sizeof(prefix) + 30);
            chosenVoice.append("Microsoft Server Speech Text to Speech Voice ");

            // Set default voice based on language
            auto it = std::find_if(languageToDefaultVoice.begin(), languageToDefaultVoice.end(), [&chosenLanguage](const tuple_type& item)
            {
                const auto lang = std::get<0>(item);
                return PAL::stricmp(lang, chosenLanguage.c_str()) == 0;
            });
            if (it != languageToDefaultVoice.end())
            {
                chosenVoice.append(std::get<1>(*it));
            }
            else
            {
                // If it's not found, use en-US default voice
                chosenVoice.append("(en-US, AriaRUS)");
            }
        }

        std::ostringstream oss;
        oss << "<speak version='1.0' xmlns='http://www.w3.org/2001/10/synthesis' xmlns:mstts='http://www.w3.org/2001/mstts' xmlns:emo='http://www.w3.org/2009/10/emotionml' xml:lang='";
        oss << chosenLanguage << "'>";
        if (!chosenVoice.empty())
        {
            oss << "<voice name='" << chosenVoice << "'>";
        }
        oss << XmlEncode(text);
        if (!chosenVoice.empty())
        {
            oss << "</voice>";
        }
        oss << "</speak>";

        return oss.str();
    };

    static std::string XmlEncode(const std::string& text)
    {
        std::stringstream ss;
        for (char c : text)
        {
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

    static std::shared_ptr<std::vector<uint8_t>> BuildRiffHeader(uint32_t cData, uint32_t cEventData, SpxWAVEFORMATEX_Type audioFormat)
    {
        RIFFHDR riff(0);
        BLOCKHDR block(0);
        DATAHDR dataHdr(0);

        uint32_t cRiff = sizeof(riff);
        uint32_t cBlock = sizeof(block);
        uint32_t cWaveEx = 18 + audioFormat->cbSize; // use 18 for actual size to avoid compiler alignment difference.
        uint32_t cDataHdr = sizeof(dataHdr);

        uint32_t total = cRiff + cBlock + cWaveEx + cDataHdr;
        if (audioFormat->wFormatTag == WAVE_FORMAT_SIREN)
        {
            total += 12;
        }

        if (cEventData > 0)
        {
            total += (8 + cEventData);
        }

        uint8_t tmpBuf[128];
        uint8_t* p = tmpBuf;
        // Write the RIFF section
        riff._len = total + cData - 8/* - cRiff*/; // for the "WAVE" 4 characters
        buffer_write(&p, riff._id);
        buffer_write(&p, riff._len);
        buffer_write(&p, riff._type);

        // Write the wave header section
        block._len = cWaveEx;
        buffer_write(&p, block._id);
        buffer_write(&p, block._len);

        // Write the FormatEx structure
        buffer_write(&p, audioFormat->wFormatTag);
        buffer_write(&p, audioFormat->nChannels);
        buffer_write(&p, audioFormat->nSamplesPerSec);
        buffer_write(&p, audioFormat->nAvgBytesPerSec);
        buffer_write(&p, audioFormat->nBlockAlign);
        buffer_write(&p, audioFormat->wBitsPerSample);
        buffer_write(&p, audioFormat->cbSize);

        if (audioFormat->wFormatTag == WAVE_FORMAT_SIREN)
        {
            buffer_write(&p, static_cast<uint16_t>(320));
            buffer_write(&p, 'f');
            buffer_write(&p, 'a');
            buffer_write(&p, 'c');
            buffer_write(&p, 't');
            buffer_write(&p, static_cast<uint32_t>(4));
            uint32_t factSize = (cData * 320) / audioFormat->nBlockAlign;
            buffer_write(&p, factSize);
        }

        // Write the data section
        dataHdr._len = cData;
        buffer_write(&p, dataHdr._id);
        buffer_write(&p, dataHdr._len);

        return std::make_shared<std::vector<uint8_t>>(tmpBuf, p);
    }

    static bool LanguageAutoDetectionEnabled(const std::shared_ptr<ISpxNamedProperties>& properties)
    {
        const auto autoDetectSourceLanguages = properties->GetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_AutoDetectSourceLanguages));
        return g_autoDetectSourceLang_OpenRange == autoDetectSourceLanguages;
    }
};


} } } } // Microsoft::CognitiveServices::Speech::Impl
