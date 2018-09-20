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

typedef std::unordered_map<PropertyId, const char*, EnumClassHash > SpeechPropertyIdNameHash;

const SpeechPropertyIdNameHash g_PropertyId2NameMap = {
    { PropertyId::SpeechServiceConnection_Key, "SPEECH-SubscriptionKey" },
    { PropertyId::SpeechServiceConnection_Endpoint, "SPEECH-Endpoint" },
    { PropertyId::SpeechServiceConnection_Region, "SPEECH-Region" },
    { PropertyId::SpeechServiceAuthorization_Token, "SPEECH-AuthToken" },
    { PropertyId::SpeechServiceAuthorization_Type, "SpeechServiceAuthorization_Type" },
    { PropertyId::SpeechServiceConnection_EndpointId, "SPEECH-ModelId" },
    { PropertyId::SpeechServiceConnection_TranslationToLanguages, "TRANSLATION-ToLanguages" },
    { PropertyId::SpeechServiceConnection_TranslationVoice, "TRANSLATION-Voice" },
    { PropertyId::SpeechServiceConnection_TranslationFeatures, "TRANSLATION-Features" },
    { PropertyId::SpeechServiceConnection_IntentRegion, "INTENT-region" },
    { PropertyId::SpeechServiceConnection_RecoMode, "SPEECH-RecoMode" },
    { PropertyId::SpeechServiceConnection_RecoLanguage, "SPEECH-RecoLanguage" },
    { PropertyId::Speech_SessionId, "SessionId" },
    { PropertyId::SpeechServiceResponse_RequestDetailedResultTrueFalse, "SpeechServiceResponse_RequestDetailedResultTrueFalse" },
    { PropertyId::SpeechServiceResponse_RequestProfanityFilterTrueFalse, "SpeechServiceResponse_RequestProfanityFilterTrueFalse" },
    { PropertyId::SpeechServiceResponse_JsonResult, "RESULT-Json" },
    { PropertyId::LanguageUnderstandingServiceResponse_JsonResult, "RESULT-LanguageUnderstandingJson" },
    { PropertyId::SpeechServiceResponse_JsonErrorDetails, "RESULT-ErrorDetails" },
    { PropertyId::CancellationDetails_Reason, "CancellationDetails_Reason" },
    { PropertyId::CancellationDetails_ReasonText, "CancellationDetails_ReasonText" },
    { PropertyId::CancellationDetails_ReasonDetailedText, "CancellationDetails_ReasonDetailedText" }
};

const char* GetPropertyName(const PropertyId& id)
{
    if (g_PropertyId2NameMap.find(id) != g_PropertyId2NameMap.end())
    {
        return g_PropertyId2NameMap.at(id);
    }
    else
    {
        LogError("undefined PropertyId of %d", static_cast<int>(id));
        SPX_THROW_ON_FAIL(SPXERR_INVALID_ARG);
        return "";
    }
}
}}}} // Microsoft::CognitiveServices::Speech::Impl
