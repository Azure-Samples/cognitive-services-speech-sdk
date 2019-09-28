//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//

#include "stdafx.h"
#include "language_list_utils.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {
    void CSpxLanguageListUtils::AddLangToList(const std::string& lang, std::string& languageList)
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

    void CSpxLanguageListUtils::RemoveLangFromList(const std::string& lang, std::string& languageList)
    {
        if (lang.empty() || lang.find(CommaDelim) != std::string::npos)
        {
            ThrowInvalidArgumentException("Only one non-empty language name is allowed.");
        }

        auto pos = languageList.find(lang);
        if (pos == std::string::npos)
        {
            SPX_DBG_TRACE_WARNING("%s: The language to be removed %s is not in target languages: %s", __FUNCTION__, lang.c_str(), languageList.c_str());
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
}}}} // Microsoft::CognitiveServices::Speech::Impl
