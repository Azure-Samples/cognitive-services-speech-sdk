//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See https://aka.ms/csspeech/license for the full license information.
//

#include <regex>
#include <stack>
#include <stdlib.h>

#include "intent_trigger.h"
#include "list_entity.h"
#include "pattern_matching_utils.h"
#include "string_utils.h"

namespace Microsoft {
namespace SpeechSDK {
namespace Standalone {
namespace Intent {
namespace Impl {

void CSpxListEntity::Init(const std::string& name, const OrthographyInformation& orthography)
{
    m_name = name;
    m_orthography = &orthography;
    m_greed = 0;
}

void CSpxListEntity::SetMode(Intent::EntityMatchMode mode)
{
    m_matchMode = mode;
}

void CSpxListEntity::AddPhrase(const std::string& phrase)
{
    std::string normalized = CSpxIntentTrigger::NormalizeInput(phrase);
    if (!normalized.empty())
    {
        m_phrases.push_back(normalized);

        // Update the greed based on the larges number of words in our list of phrases
        m_greed = std::max(m_greed, CalculateGreed(*m_orthography, normalized));
    }
}

const std::string& CSpxListEntity::GetName() const
{
    return m_name;
}

Intent::EntityMatchMode CSpxListEntity::GetMode() const
{
    return Intent::EntityMatchMode::Basic;
}

Intent::EntityType CSpxListEntity::GetType() const
{
    return Intent::EntityType::List;
}

unsigned int CSpxListEntity::CalculateGreed(const OrthographyInformation& orthography, const std::string& phrase)
{
    const char* inputCopy = phrase.c_str();
    unsigned int num = 0;
    while (!Utils::GrabNextWord(&inputCopy, orthography).empty())
    {
        num++;
    }
    return num;
}

Maybe<std::string> CSpxListEntity::Parse(const std::string& input) const
{
    std::string result(input);
    // If we are strict default to an empty string.
    if (m_matchMode == Intent::EntityMatchMode::Strict)
    {
        result = "";
        for (auto& phrase : m_phrases)
        {
            if (PAL::stricmp(phrase.c_str(), input.c_str()) == 0)
            {
                return phrase;
            }
        }
    }
    else
    {
        return input;
    }

    // This means we are in strict mode, but didn't find anything.
    return Maybe<std::string>();
}

}}}}}
