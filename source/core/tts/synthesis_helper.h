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



#define ISSUE_TOKEN_HOST_SUFFIX ".api.cognitive.microsoft.com"
#define ISSUE_TOKEN_URL_PATH "/sts/v1.0/issueToken"
#define TTS_COGNITIVE_SERVICE_HOST_SUFFIX ".tts.speech.microsoft.com"
#define TTS_COGNITIVE_SERVICE_URL_PATH "/cognitiveservices/v1"
#define TTS_COGNITIVE_SERVICE_WSS_URL_PATH "/cognitiveservices/websocket/v1"
#define TTS_CUSTOM_VOICE_HOST_SUFFIX ".voice.speech.microsoft.com"
#define USER_AGENT "SpeechSDK"

#define SSML_TEMPLATE "<speak version='1.0' xmlns='http://www.w3.org/2001/10/synthesis' xmlns:mstts='http://www.w3.org/2001/mstts' xmlns:emo='http://www.w3.org/2009/10/emotionml' xml:lang='%s'><voice name='%s'>%s</voice></speak>"
#define SSML_BUFFER_SIZE 0x10000

#define BUFFERWRITE(buf, value) for (size_t i = 0; i < sizeof(value); ++i) { *buf = (uint8_t)((value >> (i * 8)) & 0xff); ++buf; }

typedef uint32_t UINT32;

const UINT32 RIFF_MARKER = 0x46464952;
const UINT32 WAVE_MARKER = 0x45564157;
const UINT32 FMT_MARKER = 0x20746d66;
const UINT32 DATA_MARKER = 0x61746164;
const UINT32 EVNT_MARKER = 0x544e5645;

struct RIFFHDR
{
    UINT32 _id;
    UINT32 _len;              /* file length less header */
    UINT32 _type;            /* should be "WAVE" */

    RIFFHDR(UINT32 length)
    {
        _id = RIFF_MARKER;
        _type = WAVE_MARKER;
        _len = length;
    }
};

struct BLOCKHDR
{
    UINT32 _id;              /* should be "fmt " or "data" */
    UINT32 _len;              /* block size less header */

    BLOCKHDR(int length)
    {
        _id = FMT_MARKER;
        _len = length;
    }
};

struct DATAHDR
{
    UINT32 _id;               /* should be "fmt " or "data" */
    UINT32 _len;              /* block size less header */

    DATAHDR(UINT32 length)
    {
        _id = DATA_MARKER;
        _len = length;
    }
};

struct EVNTHDR
{
    UINT32 _id;               /* should be "EVNT" */
    UINT32 _len;              /* block size less header */

    EVNTHDR(UINT32 length)
    {
        _id = EVNT_MARKER;
        _len = length;
    }
};


namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

class CSpxSynthesisHelper
{
public:

    static std::string itos(size_t i)
    {
        std::stringstream ss;
        ss << i;
        return ss.str();
    };

    static std::string ParseRegionFromCognitiveServiceEndpoint(const std::string& endpoint)
    {
        auto url = HttpUtils::ParseUrl(endpoint);

        size_t first_dot_pos = url.host.find('.');
        if (first_dot_pos == size_t(-1))
        {
            ThrowRuntimeError("The given endpoint is not valid TTS cognitive service endpoint. Expected *.tts.speech.microsoft.com or *.voice.speech.microsoft.com");
        }

        std::string host_name_suffix = url.host.substr(first_dot_pos);
        if (PAL::stricmp(host_name_suffix.data(), TTS_COGNITIVE_SERVICE_HOST_SUFFIX) != 0 &&
            PAL::stricmp(host_name_suffix.data(), TTS_CUSTOM_VOICE_HOST_SUFFIX) != 0)
        {
            ThrowRuntimeError("The given endpoint is not valid TTS cognitive service endpoint. Expected *.tts.speech.microsoft.com or *.voice.speech.microsoft.com");
        }

        return url.host.substr(0, first_dot_pos);
    }

    static bool IsCustomVoiceEndpoint(const std::string& endpoint)
    {
        auto url = HttpUtils::ParseUrl(endpoint);

        size_t first_dot_pos = url.host.find('.');
        if (first_dot_pos == size_t(-1))
        {
            return false;
        }

        std::string host_name_suffix = url.host.substr(first_dot_pos);

        if (PAL::stricmp(host_name_suffix.data(), TTS_CUSTOM_VOICE_HOST_SUFFIX) == 0)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    static bool IsStandardVoiceEndpoint(const std::string& endpoint)
    {
        auto url = HttpUtils::ParseUrl(endpoint);

        size_t first_dot_pos = url.host.find('.');
        if (first_dot_pos == size_t(-1))
        {
            return false;
        }

        std::string host_name_suffix = url.host.substr(first_dot_pos);

        if (PAL::stricmp(host_name_suffix.data(), TTS_COGNITIVE_SERVICE_HOST_SUFFIX) == 0)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    static std::string BuildSsml(const std::string& text, const std::string& language, const std::string& voice)
    {
        std::map<const char*, const char*> languageToDefaultVoice = {
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
            // If it's not found, use en-US default voice
            chosenVoice = "Microsoft Server Speech Text to Speech Voice (en-US, JessaRUS)";

            // Set default voice based on language
            for (auto item : languageToDefaultVoice)
            {
                if (PAL::stricmp(item.first, chosenLanguage.c_str()) == 0)
                {
                    chosenVoice = item.second;
                    break;
                }
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
        BUFFERWRITE(p, riff._id);
        BUFFERWRITE(p, riff._len);
        BUFFERWRITE(p, riff._type);

        // Write the wave header section
        block._len = cWaveEx;
        BUFFERWRITE(p, block._id);
        BUFFERWRITE(p, block._len);

        // Write the FormatEx structure
        BUFFERWRITE(p, audioFormat->wFormatTag);
        BUFFERWRITE(p, audioFormat->nChannels);
        BUFFERWRITE(p, audioFormat->nSamplesPerSec);
        BUFFERWRITE(p, audioFormat->nAvgBytesPerSec);
        BUFFERWRITE(p, audioFormat->nBlockAlign);
        BUFFERWRITE(p, audioFormat->wBitsPerSample);
        BUFFERWRITE(p, audioFormat->cbSize);

        if (audioFormat->wFormatTag == WAVE_FORMAT_SIREN)
        {
            BUFFERWRITE(p, (uint16_t)320);
            BUFFERWRITE(p, 'f');
            BUFFERWRITE(p, 'a');
            BUFFERWRITE(p, 'c');
            BUFFERWRITE(p, 't');
            BUFFERWRITE(p, (uint32_t)4);
            uint32_t factSize = (cData * 320) / audioFormat->nBlockAlign;
            BUFFERWRITE(p, factSize);
        }

        // Write the data section
        dataHdr._len = cData;
        BUFFERWRITE(p, dataHdr._id);
        BUFFERWRITE(p, dataHdr._len);

        return std::make_shared<std::vector<uint8_t>>(tmpBuf, p);
    }
};


} } } } // Microsoft::CognitiveServices::Speech::Impl
