//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#include "stdafx.h"
#include "speech_translation_config.h"
#include "property_id_2_name_map.h"
#include "usp.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

using namespace std;

void CSpxSpeechTranslationConfig::AddLangToList(const std::string& lang, std::string& languageList)
{
    if (lang.empty() || lang.find(CommaDelim) != std::string::npos)
    {
        ThrowInvalidArgumentException("Only one non-empty language name is allowed.");
    }

    if (languageList.find(lang) != std::string::npos)
    {
        SPX_DBG_TRACE_WARNING("%s: The language to be added %s already in target lanugages: %s", __FUNCTION__, lang.c_str(), languageList.c_str());
    }
    else
    {
        if (languageList.empty())
        {
            languageList = lang;
        }
        else
        {
            languageList += std::string(1, CommaDelim) + lang;
        }
    }
}

void CSpxSpeechTranslationConfig::RemoveLangFromList(const std::string& lang, std::string& languageList)
{
    if (lang.empty() || lang.find(CommaDelim) != std::string::npos)
    {
        ThrowInvalidArgumentException("Only one non-empty language name is allowed.");
    }

    auto pos = languageList.find(lang);
    if (pos == std::string::npos)
    {
        SPX_DBG_TRACE_WARNING("%s: The language to be removed %s is not in target lanugages: %s", __FUNCTION__, lang.c_str(), languageList.c_str());
    }
    else
    {
        std::ostringstream oss;
        auto langVector = PAL::split(languageList, CommaDelim);
        bool firstItem = true;
        for (auto item : langVector)
        {
            if (item == lang)
            {
                continue;
            }
            if (firstItem)
            {
                oss << item;
                firstItem = false;
            }
            else
            {
                oss << CommaDelim << item;
            }
        }
        languageList = oss.str();
    }
}

void CSpxSpeechTranslationConfig::AddTargetLanguage(const std::string& lang)
{
    auto languageList = GetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_TranslationToLanguages), "");
    AddLangToList(lang, languageList);
    SPX_DBG_TRACE_INFO("%s: new target languages: %s", __FUNCTION__, languageList.c_str());
    SetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_TranslationToLanguages), languageList.c_str());
}

void CSpxSpeechTranslationConfig::RemoveTargetLanguage(const std::string& lang)
{
    auto languageList = GetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_TranslationToLanguages), "");
    RemoveLangFromList(lang, languageList);
    SPX_DBG_TRACE_INFO("%s: new target languages: %s", __FUNCTION__, languageList.c_str());
    SetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_TranslationToLanguages), languageList.c_str());
}

} } } } // Microsoft::CognitiveServices::Speech::Impl
