//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//

#include "stdafx.h"
#include "source_lang_config.h"
#include "property_id_2_name_map.h"
#include "usp.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

using namespace std;

void CSpxSourceLanguageConfig::InitFromLanguage(const char* language)
{
    SPX_IFTRUE_THROW_HR(m_init, SPXERR_ALREADY_INITIALIZED);
    m_init = true;
    SetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_RecoLanguage), language);
}

void CSpxSourceLanguageConfig::InitFromLanguageAndEndpointId(const char* language, const char* endpointId)
{
    InitFromLanguage(language);
    SetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_EndpointId), endpointId);
}

std::string CSpxSourceLanguageConfig::GetLanguage()
{
    SPX_IFTRUE_THROW_HR(!m_init, SPXERR_UNINITIALIZED);
    return GetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_RecoLanguage), "");
}

std::string CSpxSourceLanguageConfig::GetEndpointId()
{
    SPX_IFTRUE_THROW_HR(!m_init, SPXERR_UNINITIALIZED);
    return GetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_EndpointId), "");
}
} } } } // Microsoft::CognitiveServices::Speech::Impl
