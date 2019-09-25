//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#include "stdafx.h"
#include "speech_translation_config.h"
#include "property_id_2_name_map.h"
#include "usp.h"
#include <sstream>
#include "language_list_utils.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

using namespace std;

void CSpxSpeechTranslationConfig::AddTargetLanguage(const std::string& lang)
{
    auto languageList = GetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_TranslationToLanguages), "");
    CSpxLanguageListUtils::AddLangToList(lang, languageList);
    SPX_DBG_TRACE_INFO("%s: new target languages: %s", __FUNCTION__, languageList.c_str());
    SetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_TranslationToLanguages), languageList.c_str());
}

void CSpxSpeechTranslationConfig::RemoveTargetLanguage(const std::string& lang)
{
    auto languageList = GetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_TranslationToLanguages), "");
    CSpxLanguageListUtils::RemoveLangFromList(lang, languageList);
    SPX_DBG_TRACE_INFO("%s: new target languages: %s", __FUNCTION__, languageList.c_str());
    SetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_TranslationToLanguages), languageList.c_str());
}

} } } } // Microsoft::CognitiveServices::Speech::Impl
