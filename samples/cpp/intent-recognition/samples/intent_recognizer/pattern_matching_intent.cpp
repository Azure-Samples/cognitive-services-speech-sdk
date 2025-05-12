//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See https://aka.ms/csspeech/license for the full license information.
//

#include "stdafx.h"

#include <regex>
#include <stack>
#include <sstream>

#include "intent_trigger.h"
#include "locale_information.h"
#include "pattern_matching_intent.h"
#include "string_utils.h"

namespace Microsoft {
namespace SpeechSDK {
namespace Standalone {
namespace Intent {
namespace Impl {

void CSpxPatternMatchingIntent::Init(const std::string& id, unsigned int priority, const std::string& orthographyPrefix)
{
    m_id = id;
    m_priority = priority;
    m_orthographyPrefix = PAL::StringUtils::ToLower(orthographyPrefix);
}

void CSpxPatternMatchingIntent::AddPhrase(const std::string& phrase)
{
    if (phrase.empty())
    {
        return;
    }

    auto entities = ExtractEntities(phrase);

    auto pattern = CSpxIntentTrigger::NormalizeInput(phrase);
    if (m_orthographyPrefix == "fr")
    {
        pattern = Locales::Utils::RemoveLeadingPunctuationSpaceFR(pattern);
    }

    m_patterns.push_back({ std::move(pattern), std::move(entities), phrase });
}

std::vector<std::string> CSpxPatternMatchingIntent::ExtractEntities(const std::string& input)
{
    std::vector<std::string> entities;
    bool insideEntity = false;
    size_t start = 0;

    for (size_t i = 0; i < input.size(); i++)
    {
        char c = input[i];
        if (c == '}')
        {
            if (!insideEntity)
            {
                std::ostringstream msg;
                msg << "Invalid phrase. unmatched '}' found at " << i << " in '" << input << "'";
                SPX_TRACE_ERROR(msg.str().c_str());
                throw std::invalid_argument(msg.str());
            }
            else
            {
                insideEntity = false;
                entities.push_back(input.substr(start + 1, i - start - 1));
            }
        }
        else if (c == '{')
        {
            if (insideEntity)
            {
                std::ostringstream msg;
                msg << "Invalid phrase. double '{' found at " << i << " in '" << input << "'";
                SPX_TRACE_ERROR(msg.str().c_str());
                throw std::invalid_argument(msg.str());
            }
            else
            {
                insideEntity = true;
                start = i;
            }
        }
    }

    if (insideEntity)
    {
        std::ostringstream msg;
        msg << "Invalid phrase. unclosed '{' found at '" << input << "'";
        SPX_TRACE_ERROR(msg.str().c_str());
        throw std::invalid_argument(msg.str());
    }

    return entities;
}

}}}}}
