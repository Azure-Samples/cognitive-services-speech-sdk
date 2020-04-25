//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
//

#include <sstream>
#include "stdafx.h"
#include "translation_recognizer.h"
#include "service_helpers.h"
#include "site_helpers.h"
#include "string_utils.h"
#include "speech_translation_config.h"
#include "language_list_utils.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

CSpxTranslationRecognizer::CSpxTranslationRecognizer()
{
    SPX_DBG_TRACE_FUNCTION();
}

CSpxTranslationRecognizer::~CSpxTranslationRecognizer()
{
    SPX_DBG_TRACE_FUNCTION();
}

void CSpxTranslationRecognizer::Init()
{
    CSpxRecognizer::Init();
}

void CSpxTranslationRecognizer::AddTargetLanguage(const std::string& lang)
{
    auto properties = SpxQueryService<ISpxNamedProperties>(GetSite());
    if (properties == nullptr)
    {
        ThrowRuntimeError("Property bag object is null.");
    }

    auto targetLanguages = properties->GetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_TranslationToLanguages));
    CSpxLanguageListUtils::AddLangToList(lang, targetLanguages);
    SPX_DBG_TRACE_INFO("%s: Add target languages during recognition: %s", __FUNCTION__, targetLanguages.c_str());
    properties->SetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_TranslationToLanguages), targetLanguages.c_str());
    UpdateTargetLanguages(targetLanguages);
}

void CSpxTranslationRecognizer::RemoveTargetLanguage(const std::string& lang)
{
    auto properties = SpxQueryService<ISpxNamedProperties>(GetSite());
    if (properties == nullptr)
    {
        ThrowRuntimeError("Property bag object is null.");
    }

    auto targetLanguages = properties->GetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_TranslationToLanguages));
    CSpxLanguageListUtils::RemoveLangFromList(lang, targetLanguages);

    if (targetLanguages.empty())
    {
        ThrowInvalidArgumentException("Change target languages during recognition: the target language is empty after removal.");
    }
    SPX_DBG_TRACE_INFO("%s: Remove target languages during recognition: %s", __FUNCTION__, targetLanguages.c_str());
    properties->SetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_TranslationToLanguages), targetLanguages.c_str());
    UpdateTargetLanguages(targetLanguages);
}

void CSpxTranslationRecognizer::UpdateTargetLanguages(const std::string& targetLanguages)
{
    auto toArray = nlohmann::json::array();
    auto langVector = PAL::split(targetLanguages, CommaDelim);
    for (auto item : langVector)
    {
        toArray.push_back(item);
    }

    nlohmann::json eventPayload;
    eventPayload["id"] = "translation";
    eventPayload["name"] = "updateLanguage";
    eventPayload["to"] = toArray;

    auto session = GetDefaultSession();
    if (session == nullptr)
    {
        ThrowRuntimeError("UpdateTargetLanguages: the session object is nullptr.");
    }
    session->SendNetworkMessage("event", eventPayload.dump(), false);
}

} } } } // Microsoft::CognitiveServices::Speech::Impl
