//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//

#include "stdafx.h"
#include "auto_detect_source_lang_config.h"
#include "language_list_utils.h"
#include "property_id_2_name_map.h"
#include "usp.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

using namespace std;

void CSpxAutoDetectSourceLangConfig::InitFromOpenRange()
{
    SPX_IFTRUE_THROW_HR(m_init, SPXERR_ALREADY_INITIALIZED);
    m_init = true;
    SetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_AutoDetectSourceLanguages), g_autoDetectSourceLang_OpenRange);
}

void CSpxAutoDetectSourceLangConfig::InitFromLanguages(const char* languages)
{
    SPX_IFTRUE_THROW_HR(m_init, SPXERR_ALREADY_INITIALIZED);
    m_init = true;
    SetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_AutoDetectSourceLanguages), languages);
}

void CSpxAutoDetectSourceLangConfig::AddSourceLanguageConfig(std::shared_ptr<ISpxSourceLanguageConfig> sourceLanguageConfig)
{
    std::string languageList = GetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_AutoDetectSourceLanguages), "");
    auto language = sourceLanguageConfig->GetLanguage();
    CSpxLanguageListUtils::AddLangToList(language, languageList);
    SetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_AutoDetectSourceLanguages), languageList.c_str());
    SPX_DBG_TRACE_INFO("%s: auto detected source languages: %s", __FUNCTION__, languageList.c_str());
    auto endpointId = sourceLanguageConfig->GetEndpointId();
    if (!endpointId.empty())
    {
        std::string endpointIdProperty = language + (string)GetPropertyName(PropertyId::SpeechServiceConnection_EndpointId);
        SetStringValue(endpointIdProperty.c_str(), endpointId.c_str());
    }
}
} } } } // Microsoft::CognitiveServices::Speech::Impl
