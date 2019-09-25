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

void CSpxAutoDetectSourceLangConfig::InitFromLanguages(const char* languages)
{
    SPX_IFTRUE_THROW_HR(m_init, SPXERR_ALREADY_INITIALIZED);
    m_init = true;
    SetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_AutoDetectSourceLanguages), languages);
}
} } } } // Microsoft::CognitiveServices::Speech::Impl
