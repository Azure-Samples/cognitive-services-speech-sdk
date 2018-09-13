//  
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// property_id_2_name_map.cpp: implemenation of mapping id to its name.
//

#include <unordered_map>

#include "stdafx.h"
#include "speechapi_cxx_properties.h"
#include "property_id_2_name_map.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

// a hash function to make gcc happy.
struct EnumClassHash
{
    template <typename T>
    std::size_t operator()(T t) const
    {
        return static_cast<std::size_t>(t);
    }
};

typedef std::unordered_map<SpeechPropertyId, const char*, EnumClassHash > SpeechPropertyIdNameHash;

const SpeechPropertyIdNameHash g_PropertyId2NameMap = {
    { SpeechPropertyId::SpeechServiceConnection_Key, "SPEECH-SubscriptionKey" },
    { SpeechPropertyId::SpeechServiceConnection_Endpoint, "SPEECH-Endpoint" },
    { SpeechPropertyId::SpeechServiceConnection_Region, "SPEECH-Region" },
    { SpeechPropertyId::SpeechServiceAuthorization_Token, "SPEECH-AuthToken" },
    { SpeechPropertyId::SpeechServiceAuthorization_Type, "SpeechServiceAuthorization_Type" },
    { SpeechPropertyId::SpeechServiceRps_Token, "SPEECH-RpsToken" },
    { SpeechPropertyId::SpeechServiceConnection_EndpointId, "SPEECH-ModelId" },
    { SpeechPropertyId::SpeechServiceConnection_TranslationFromLanguage, "TRANSLATION-FromLanguage" },
    { SpeechPropertyId::SpeechServiceConnection_TranslationToLanguages, "TRANSLATION-ToLanguages" },
    { SpeechPropertyId::SpeechServiceConnection_TranslationVoice, "TRANSLATION-Voice" },
    { SpeechPropertyId::SpeechServiceConnection_TranslationFeatures, "TRANSLATION-Features" },
    { SpeechPropertyId::SpeechServiceConnection_IntentSourceLanguage, "INTENT-Lang" },
    { SpeechPropertyId::SpeechServiceConnection_IntentRegion, "INTENT-region" },
    { SpeechPropertyId::SpeechServiceConnection_RecoMode, "SPEECH-RecoMode" },
    { SpeechPropertyId::SpeechServiceConnection_RecoMode_Interactive, "INTERACTIVE" },
    { SpeechPropertyId::SpeechServiceConnection_RecoMode_Conversation, "CONVERSATION" },
    { SpeechPropertyId::SpeechServiceConnection_RecoMode_Dictation, "DICTATION" },
    { SpeechPropertyId::SpeechServiceConnection_RecoLanguage, "SPEECH-RecoLanguage" },
    { SpeechPropertyId::Speech_SessionId, "SessionId" },
    { SpeechPropertyId::SpeechServiceResponse_RequestDetailedResultTrueFalse, "SpeechServiceResponse_RequestDetailedResultTrueFalse" },
    { SpeechPropertyId::SpeechServiceResponse_RequestProfanityFilterTrueFalse, "SpeechServiceResponse_RequestProfanityFilterTrueFalse" },
    { SpeechPropertyId::SpeechServiceResponse_Json, "RESULT-Json" },
    { SpeechPropertyId::SpeechServiceResponse_JsonResult, "RESULT-LanguageUnderstandingJson" },
    { SpeechPropertyId::SpeechServiceResponse_JsonErrorDetails, "RESULT-ErrorDetails" },
    { SpeechPropertyId::CancellationDetails_ReasonCanceled, "CancellationDetails_ReasonCanceled" },
    { SpeechPropertyId::CancellationDetails_ReasonText, "CancellationDetails_ReasonText" },
    { SpeechPropertyId::CancellationDetails_ReasonDetailedText, "CancellationDetails_ReasonDetailedText" }
};

const char* GetPropertyName(const SpeechPropertyId& id)
{
    if (g_PropertyId2NameMap.find(id) != g_PropertyId2NameMap.end())
    {
        return g_PropertyId2NameMap.at(id);
    }
    else
    {
        LogError("undefined SpeechPropertyId of %d", static_cast<int>(id));
        SPX_THROW_ON_FAIL(SPXERR_INVALID_ARG);
        return "";
    }
}
}}}} // Microsoft::CognitiveServices::Speech::Impl
