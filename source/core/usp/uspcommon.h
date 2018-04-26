//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// uspcommon.h: common definitions and declaration used by USP internal implementation
//

#pragma once

#include "usperror.h"

#define UNUSED(x) (void)(x)

#define USP_RETURN_NOT_IMPLEMENTED() \
    do { \
        LogError("Not implemented"); \
        return USP_NOT_IMPLEMENTED; \
    } while (0)

#define USE_BUFFER_SIZE    ((size_t)-1)

#if defined _MSC_VER
#define PROTOCOL_VIOLATION(__fmt, ...)  LogError("ProtocolViolation:" __fmt, __VA_ARGS__)
#else
#define PROTOCOL_VIOLATION(__fmt, ...)  LogError("ProtocolViolation:" __fmt, ##__VA_ARGS__)
#endif

#ifdef __cplusplus

#include <string>

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace USP {

    namespace endpoint
    {
        const std::string protocol = "wss://";

        const std::string pathPrefix = "/speech/recognition/";
        const std::string pathSuffix = "/cognitiveservices/v1?";
        const std::string langQueryParam = "language=";

        namespace translation
        {
            const std::string path = "/speech/translation/cognitiveservices/v1?";
            const std::string from = "from=";
            const std::string to = "to=";
            const std::string voice = "voice=";
            const std::string features = "features=";
            const std::string requireVoice = "texttospeech";
        }

        namespace hostname 
        {
            // Todo: update all hostnames with new unified speech hosts.
            const std::string bingSpeech = "speech.platform.bing.com";
            const std::string CRIS = ".api.cris.ai";
            const std::string Translation = "fedev.develop2.cris.ai";
            const std::string CDSDK = "speech.platform.bing.com/cortana/api/v1?environment=Home&";
        }
    }

    namespace path {
        const std::string speechHypothesis = "speech.hypothesis";
        const std::string speechPhrase = "speech.phrase";
        const std::string speechFragment = "speech.fragment";
        const std::string turnStart = "turn.start";
        const std::string turnEnd = "turn.end";
        const std::string speechStartDetected = "speech.startDetected";
        const std::string speechEndDetected = "speech.endDetected";
        const std::string translationHypothesis = "translation.hypothesis";
        const std::string translationPhrase = "translation.phrase";
        const std::string translationSynthesis = "translation.synthesis";
        const std::string translationSynthesisEnd = "translation.synthesis.end";
    }
    //Todo: Figure out what to do about user agent build hash and version number
    const auto g_userAgent = "CortanaSDK (Windows;Win32;DeviceType=Near;SpeechClient=2.0.4)";

    namespace headers {
        const auto userAgent = "User-Agent";
        const auto ocpApimSubscriptionKey = "Ocp-Apim-Subscription-Key";
        const auto authorization = "Authorization";
        const auto searchDelegationRPSToken = "X-Search-DelegationRPSToken";
        const auto audioResponseFormat = "X-Output-AudioCodec";
        const auto contentType = "Content-Type";
    }

    namespace json_properties {
        const std::string offset = "Offset";
        const std::string duration = "Duration";
        const std::string text = "Text";
        const std::string recoStatus = "RecognitionStatus";
        const std::string displayText = "DisplayText";
        const std::string context = "context";
        const std::string tag = "serviceTag";

        const std::string translation = "Translation";
        const std::string translationStatus = "TranslationStatus";
        const std::string translations = "Translations";
        const std::string lang = "Language";
    }

}
}
}
}
#else

extern const char* g_keywordContentType;
extern const char* g_messagePathSpeechHypothesis;
extern const char* g_messagePathSpeechPhrase;
extern const char* g_messagePathSpeechFragment;
extern const char* g_messagePathTurnStart;
extern const char* g_messagePathTurnEnd;
extern const char* g_messagePathSpeechEndDetected;
extern const char* g_messagePathSpeechStartDetected;

#endif



#ifdef __cplusplus
extern "C" {
#endif

    typedef struct BUFFER_TAG* BUFFER_HANDLE;
    typedef struct _IOBUFFER IOBUFFER;

    /**
    * Handles response messages from service based on content type.
    * @param context The content context.
    * @param path The content path.
    * @param mime The content type.
    * @param ioBuffer The pointer to ioBuffer.
    * @param responseContent The content buffer of the response.
    * @param responseSize The size of responseContent.
    * @return A UspResult indicating success or error.
    */
    UspResult ContentDispatch(
        void* context,
        const char* path,
        const char* mime,
        IOBUFFER* ioBuffer,
        BUFFER_HANDLE responseContent,
        size_t responseSize);

#ifdef __cplusplus
}
#endif
