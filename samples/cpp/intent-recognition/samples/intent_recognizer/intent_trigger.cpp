//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See https://aka.ms/csspeech/license for the full license information.
//

#include "stdafx.h"

#include <regex>
#include <list>
#include <ctype.h>
#include <locale>

#include "intent_trigger.h"

namespace Microsoft {
namespace SpeechSDK {
namespace Standalone {
namespace Intent {
namespace Impl {

void CSpxIntentTrigger::InitPhraseTrigger(const std::string& phrase)
{
    if (!m_intentName.empty() || m_model != nullptr || !m_phrase.empty())
    {
        throw std::runtime_error("CSpxIntentTrigger: Phrase trigger already initialized");
    }
    m_phrase = phrase;
}

void CSpxIntentTrigger::InitPatternMatchingModelTrigger(const std::shared_ptr<CSpxPatternMatchingModel>& model)
{
    if (!m_intentName.empty() || m_model != nullptr || !m_phrase.empty())
    {
        throw std::runtime_error("CSpxIntentTrigger: Pattern matching model trigger already initialized");
    }
    m_model = model;
    m_intentName = std::string();
}

/* static */ std::string CSpxIntentTrigger::NormalizeInput(const std::string& inputText)
{
    // This is a simple normalization function that does the following:
    // - Removes all trailing whitespace from the start of the string
    // - Removes all trailing whitespace from the end of string
    // - Replaces all whitespace sequences with a single space in the middle of string
    // - Converts all characters to lower case
    std::string outputText(inputText.size(), '-');

    size_t offset = 0;
    bool prevWasSpace = true;

    for (size_t i = 0; i < inputText.size(); i++)
    {
        char c = inputText[i];
        if (std::isspace((unsigned char)c))
        {
            prevWasSpace = true;
        }
        else
        {
            if (prevWasSpace && offset > 0)
            {
                outputText[offset++] = ' ';
            }

            outputText[offset++] = std::tolower(c, std::locale::classic());
            prevWasSpace = false;
        }
    }

    outputText.resize(offset);
    return outputText;
}

}}}}}
